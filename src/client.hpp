#pragma once
#include "types.hpp"

#include "bytebuffer.hpp"

#include <boost/asio/io_service.hpp>
#include <vector>

using std::vector;

namespace Impl
{
    class Session;
}

class Client
{
public:
    explicit Client(const string& host, u32 port);
    void Connect();
    void Disconnect();
    const string& GetAuth() const { return m_auth; }
    const string& GetUser() const { return m_user; }
    void Write(const ByteBuffer& data);
    void Read(ByteBuffer& data);

private:
    string m_host;
    u32 m_port;
    string m_auth;
    string m_user;
    shared_ptr<Impl::Session> m_session;
    boost::asio::io_context m_ioc;
};