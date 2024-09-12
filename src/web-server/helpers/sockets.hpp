#ifndef HELPERS_SOCKETS_HPP
#define HELPERS_SOCKETS_HPP

#include <asio/ip/address.hpp>
#include <asio/ip/tcp.hpp>

namespace web_server::helpers {
    using Acceptor = asio::ip::tcp::acceptor;
    using Listener = Acceptor;

    using LocalEndPoint = asio::ip::tcp::endpoint;
    using RemoteAddress = asio::ip::address;
    using Port = asio::ip::port_type;

    using Socket = asio::ip::tcp::socket;
    using TCPStream = Socket;

    void close(TCPStream& stream);
} // namespace web_server::helpers

#endif // HELPERS_SOCKETS_HPP
