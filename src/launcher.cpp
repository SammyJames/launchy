#include "launcher.hpp"

#include "boost/beast.hpp"
#include "tinyxml2.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <iostream>
#include <openssl/err.h>
#include <openssl/ssl.h>

namespace
{
    const char* kHost   = "launcher.returnofreckoning.com";
    const char* kPort   = "443";
    const char* kTarget = "/launcher.xml";
} // namespace

using tcp      = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace ssl  = boost::asio::ssl;     // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;   // from <boost/beast/http.hpp>
using std::make_shared;

using namespace tinyxml2;

void fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}
class Session : public std::enable_shared_from_this<Session>
{
public:
    explicit Session(boost::asio::io_context& ioc, ssl::context& ctx)
        : m_resolver(ioc)
        , m_stream(ioc, ctx)
    {
    }

    void Run()
    {
        m_request.version(11);
        m_request.method(http::verb::get);
        m_request.target(kTarget);
        m_request.set(http::field::host, kHost);
        m_request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        m_resolver.async_resolve(kHost,
                                 kPort,
                                 std::bind(&Session::OnResolve,
                                           shared_from_this(),
                                           std::placeholders::_1,
                                           std::placeholders::_2));
    }

    string GetBody() const
    {
        string result = m_response.body();
        return result;
    }

private:
    void OnResolve(boost::system::error_code ec, tcp::resolver::results_type results)
    {
        boost::asio::async_connect(
            m_stream.next_layer(),
            results.begin(),
            results.end(),
            std::bind(&Session::OnConnect, shared_from_this(), std::placeholders::_1));
    }

    void OnConnect(boost::system::error_code ec)
    {
        m_stream.async_handshake(
            ssl::stream_base::client,
            std::bind(&Session::OnHandshake, shared_from_this(), std::placeholders::_1));
    }

    void OnHandshake(boost::system::error_code ec)
    {
        http::async_write(m_stream,
                          m_request,
                          std::bind(&Session::OnWrite,
                                    shared_from_this(),
                                    std::placeholders::_1,
                                    std::placeholders::_2));
    }

    void OnWrite(boost::system::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        http::async_read(m_stream,
                         m_buffer,
                         m_response,
                         std::bind(&Session::OnRead,
                                   shared_from_this(),
                                   std::placeholders::_1,
                                   std::placeholders::_2));
    }

    void OnRead(boost::system::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);
        m_stream.async_shutdown(
            std::bind(&Session::OnShutdown, shared_from_this(), std::placeholders::_1));
    }

    void OnShutdown(boost::system::error_code ec)
    {
        if (ec == boost::asio::error::eof)
        {
            ec.assign(0, ec.category());
        }
        else if (ec.category() == boost::asio::error::get_ssl_category() &&
                 ec.value() == ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SHORT_READ))
        {
            m_stream.lowest_layer().close();
            ec.assign(0, ec.category());
        }

        if (ec)
            return fail(ec, "shutdown");
    }

private:
    tcp::resolver m_resolver;
    ssl::stream<tcp::socket> m_stream;
    boost::beast::flat_buffer m_buffer; // (Must persist between reads)
    http::request<http::empty_body> m_request;
    http::response<http::string_body> m_response;
};

LauncherInfo RequestLauncherInfo()
{
    boost::asio::io_context ioc;
    ssl::context ctx{ssl::context::sslv23_client};

    auto session = make_shared<Session>(ioc, ctx);
    session->Run();
    ioc.run();

    auto body = session->GetBody();
    XMLDocument doc;
    doc.Parse(body.c_str(), body.size());

    LauncherInfo result;
    if (auto launcher = doc.FirstChildElement("Launcher"))
    {
        if (auto server_xml = launcher->FirstChildElement("LauncherServer"))
        {
            result.m_address = server_xml->FirstChildElement("Ip")->GetText();
            result.m_port    = server_xml->FirstChildElement("Port")->UnsignedText(8000);
        }
        if (auto version_xml = launcher->FirstChildElement("Version"))
        {
            result.m_version = version_xml->UnsignedText(1u);
        }
        if (auto password_xml = launcher->FirstChildElement("PasswordMode"))
        {
            result.m_password_mode = password_xml->UnsignedText(0u);
        }
    }

    return result;
}