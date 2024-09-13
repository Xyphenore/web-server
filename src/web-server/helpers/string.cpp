#include "string.hpp"

#ifdef USE_RANGES
    #include <range/v3/algorithm/all_of.hpp>
    #include <range/v3/range/conversion.hpp>
    #include <range/v3/view/transform.hpp>
#endif // USE_RANGES

#include <fmt/base.h>

#include <locale>
#include <string>

#ifndef USE_RANGES
    #include <algorithm>
    #include <execution>
    #include <tuple>
#endif // USE_RANGES

namespace web_server::helpers {
#ifndef USE_RANGES
    std::string to_uppercase(const fmt::string_view value) noexcept {
        std::string upper_value{};
        upper_value.resize(value.size());

        static const std::locale locale{};
        static const auto to_upper = [](const std::string::value_type c) { return std::toupper(c, locale); };

        std::ignore = std::transform(
    #ifdef __cpp_lib_execution
            std::execution::par_unseq,
    #endif // __cpp_lib_execution
            value.begin(),
            value.end(),
            upper_value.begin(),
            to_upper);

        return upper_value;
    }

    bool is_blank(const fmt::string_view value) noexcept {
        static const std::locale locale{};
        static const auto is_blank = [](const fmt::string_view::value_type ch) {
            return std::isblank(ch, locale) or std::isspace(ch, locale);
        };

        return std::all_of(
    #ifdef __cpp_lib_execution
            std::execution::par_unseq,
    #endif // __cpp_lib_execution
            value.begin(),
            value.end(),
            is_blank);
    }
#else // USE_RANGES
    std::string to_uppercase(const fmt::string_view value) noexcept {
        static const std::locale locale{};
        static const auto to_upper = [](const fmt::string_view::value_type c) { return std::toupper(c, locale); };

        return value | ranges::views::transform(to_upper) | ranges::to<std::string>;
    }

    bool is_blank(const fmt::string_view value) noexcept {
        static const std::locale locale{};
        static const auto is_blank = [](const fmt::string_view::value_type ch) {
            return std::isblank(ch, locale) or std::isspace(ch, locale);
        };

        return ranges::all_of(value, is_blank);
    }
#endif // USE_RANGES
} // namespace web_server::helpers
