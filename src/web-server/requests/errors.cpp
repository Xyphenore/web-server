#include "errors.hpp"

// Need to format asio::ip::address
// ReSharper disable once CppUnusedIncludeDirective
#include <web-server/helpers/compat.hpp>

#include <asio/error_code.hpp>
#include <asio/system_error.hpp>

#include <fmt/format.h>

#include <system_error>

namespace web_server::requests::errors {
    MessagePartiallySentError::MessagePartiallySentError(
        const Size missing_size,
        const Address& client_address) noexcept:
    asio::system_error{
        asio::error_code{fmt::enums::format_as(std::errc::io_error), asio::system_category()},
        fmt::format(
            FMT_STRING("A message to the client ('{}') is partially sent, the missing size: {:d} Bytes."),
            client_address,
            missing_size),
    } {}
} // namespace web_server::requests::errors
