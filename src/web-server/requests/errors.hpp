#ifndef REQUESTS_ERRORS_HPP
#define REQUESTS_ERRORS_HPP

#include <web-server/helpers/sockets.hpp>

#include <asio/system_error.hpp>

#include <cstddef>

namespace web_server::requests::errors {
    class [[nodiscard]] MessagePartiallySentError final: public asio::system_error {
        public:
            using Size = std::size_t;
            using Address = helpers::RemoteAddress;

            explicit MessagePartiallySentError(Size missing_size, const Address& client_address) noexcept;
    };
} // namespace web_server::requests::errors

#endif // REQUESTS_ERRORS_HPP
