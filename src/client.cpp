#include "client.hpp"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>

#include <cstdlib>
#include <iostream>
#include <vector>

using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
using std::enable_shared_from_this;
using std::vector;

namespace Impl
{
    class Session : public enable_shared_from_this<Session>
    {
    public:
        explicit Session(boost::asio::io_context& ioc)
            : m_resolver(ioc)
            , m_socket(ioc)
        {
        }

        void Connect(const string& host, u32 port)
        {
            char buffer[ 8 ];
            snprintf(buffer, 8, "%d", port);

            m_resolver.async_resolve(host,
                                     buffer,
                                     std::bind(&Session::OnResolve,
                                               shared_from_this(),
                                               std::placeholders::_1,
                                               std::placeholders::_2));
        }

        void Disconnect() { m_socket.close(); }

        void Write(const string& data)
        {
            boost::asio::write(m_socket, boost::asio::buffer(data.data(), data.size()));
        }

        void Read(string& data)
        {
            boost::asio::read_until(m_socket, m_buffer, '\0');
            std::copy(std::istreambuf_iterator<char>(&m_buffer),
                      std::istreambuf_iterator<char>(),
                      std::back_inserter(data));
        }

    private:
        void OnResolve(boost::system::error_code ec, tcp::resolver::results_type results)
        {
            TryConnect(results.begin());
        }

        void TryConnect(tcp::resolver::iterator iter)
        {
            if (iter != tcp::resolver::iterator())
            {
                m_socket.async_connect(
                    iter->endpoint(),
                    std::bind(
                        &Session::OnConnect, shared_from_this(), std::placeholders::_1, iter));
            }
        }

        void OnConnect(boost::system::error_code ec, tcp::resolver::iterator iter)
        {
            if (!m_socket.is_open())
            {
                TryConnect(++iter);
            }
        }

    private:
        tcp::resolver m_resolver;
        tcp::socket m_socket;
        boost::asio::streambuf m_buffer;
    };
} // namespace Impl

Client::Client(const string& host, u32 port)
    : m_host(host)
    , m_port(port)
    , m_auth()
    , m_user()
    , m_session(nullptr)
{
}

void Client::Connect()
{
    m_session = make_shared<Impl::Session>(m_ioc);
    if (m_session)
    {
        m_session->Connect(m_host, m_port);
        m_ioc.run();
    }
}

void Client::Disconnect()
{
    if (m_session)
    {
        m_session->Disconnect();
    }
}

void Client::Write(const string& data)
{
    if (m_session)
    {
        m_session->Write(data);
    }
}

void Client::Read(string& data)
{
    if (m_session)
    {
        m_session->Read(data);
    }
}