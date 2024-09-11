#ifndef HELPERS_STRING_HPP
#define HELPERS_STRING_HPP

#include <fmt/base.h>

#include <string>

namespace web_server::helpers {
    [[nodiscard]] std::string to_uppercase(fmt::string_view value) noexcept;

    [[nodiscard]] bool is_blank(fmt::string_view value) noexcept;
} // namespace web_server::helpers

#endif // HELPERS_STRING_HPP
