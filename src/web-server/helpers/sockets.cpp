#include "sockets.hpp"

#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/read.hpp>
#include <asio/system_error.hpp>

#include <string>
#include <tuple>

namespace web_server::helpers {
    void close(TCPStream& stream) {
        if (not stream.is_open()) {
            return;
        }

        stream.shutdown(TCPStream::shutdown_send);

        std::string content{};

        try {
            std::ignore = asio::read(stream, asio::dynamic_buffer(content));
        }
        catch (const asio::system_error& error) {
            if (asio::error::eof != error.code()) {
                throw;
            }
        }

        stream.close();
    }
} // namespace web_server::helpers
