#ifndef TOOLS_TOOLS_HPP
#define TOOLS_TOOLS_HPP

#include <algorithm>
#include <execution>
#include <string>
#include <string_view>
#include <locale>
#include <stdexcept>
#include <filesystem>

namespace web_server::tools {
    [[nodiscard]] inline std::string to_uppercase(const std::string_view value) {
        std::string upper_value{value};

        std::for_each(
            std::execution::seq,
            upper_value.begin(),
            upper_value.end(),
            [](char& c) { c = std::toupper(c, std::locale()); }
            );

        return upper_value;
    }

    [[nodiscard]] inline bool is_blank(const std::string_view value) {
        const auto* const last_left_blank_char = std::find_if_not(
            std::execution::par_unseq,
            value.cbegin(),
            value.cend(),
            [](const char ch) {
                return std::isblank(ch, std::locale()) || std::isspace(ch, std::locale());
            }
            );

        return last_left_blank_char == value.cend();
    }

    namespace errors {
        class ZeroSizeTValue final: public std::invalid_argument {
            public:
                ZeroSizeTValue() noexcept:
                    std::invalid_argument{"The given value is equal to 0. Please give a std::size_t greater than 0"} {}
        };
    } // namespace errors

    class NonZeroSizeT final {
        public:
            NonZeroSizeT() = delete;

            constexpr explicit NonZeroSizeT(const std::size_t value):
                value_{value} {
                if (static_cast<std::size_t>(0) == value) {
                    throw errors::ZeroSizeTValue{};
                }
            }

            [[nodiscard]] constexpr explicit operator std::size_t() const noexcept {
                return value_;
            }

        private:
            std::size_t value_;
    };

    const inline static std::filesystem::path TEMPLATES_DIRECTORY{std::filesystem::path(__FILE__).parent_path()
                                                                  .parent_path().parent_path() /= "templates"};
} // namespace web_server::tools

#endif // TOOLS_TOOLS_HPP
