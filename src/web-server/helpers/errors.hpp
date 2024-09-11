#ifndef HELPERS_ERRORS_HPP
#define HELPERS_ERRORS_HPP

#include <cerrno>
#include <string>
#include <string_view>

#ifndef errno_t
    #include <type_traits>

using errno_t = std::remove_reference_t<decltype(errno)>;
#endif

namespace web_server::helpers {
    [[noreturn]] void panic(std::string_view message) noexcept;

    [[noreturn]] void panic_due_to_a_logic_error(std::string_view message) noexcept;

    [[nodiscard]] std::string string_error(errno_t error_code) noexcept;
} // namespace web_server::helpers

#endif // HELPERS_ERRORS_HPP
