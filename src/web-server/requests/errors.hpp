#ifndef REQUESTS_ERRORS_HPP
#define REQUESTS_ERRORS_HPP

#include <string>
#include <ios>

#include <asio/ip/address.hpp>

namespace web_server::requests::errors {
    class MessageNotCompletelySendError final: public std::ios_base::failure {
        public:
            MessageNotCompletelySendError(
                const std::size_t missing_size,
                const asio::ip::address& client_address
                ) noexcept :
                std::ios_base::failure{
                    "The message to the client ('"
                    + ((client_address.to_string() += "') is not completely send, the missing size: ")
                       += std::to_string(missing_size)) += " Bytes."
                } {}
    };
} // namespace web_server::requests::errors

#endif // REQUESTS_ERRORS_HPP
