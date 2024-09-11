#include "string.hpp"

#include <fmt/base.h>

#include <algorithm>
#include <execution>
#include <locale>
#include <string>
#include <tuple>

namespace web_server::helpers {
    std::string to_uppercase(const fmt::string_view value) noexcept {
        std::string upper_value{};
        upper_value.resize(value.size());

        const std::locale locale{};

        std::ignore = std::transform(
#ifdef __cpp_lib_execution
            std::execution::par_unseq,
#endif // __cpp_lib_execution
            value.begin(),
            value.end(),
            upper_value.begin(),
            [&locale](const std::string::value_type c) { return std::toupper(c, locale); });

        return upper_value;
    }

    bool is_blank(const fmt::string_view value) noexcept {
        const std::locale locale{};

        const auto* const last_left_blank_char = std::find_if_not(
#ifdef __cpp_lib_execution
            std::execution::par_unseq,
#endif // __cpp_lib_execution
            value.begin(),
            value.end(),
            [&locale](const fmt::string_view::value_type ch) {
                return std::isblank(ch, locale) or std::isspace(ch, locale);
            });

        return last_left_blank_char == value.end();
    }
} // namespace web_server::helpers
