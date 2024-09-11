#include "errors.hpp"

#include "string.hpp"

#include <fmt/format.h>

#include <cerrno>
#include <cstring>
#include <exception>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <system_error>

namespace {
    using size_type = std::string::size_type;

    errno_t safe_strerror(std::string& error, const errno_t error_code) noexcept {
#if defined(_WIN32) or defined(WIN32) or not defined(__USE_GNU)
        static constexpr errno_t OLD_GLIBC_FAILURE{-1};

    #if defined(_WIN32) or defined(WIN32)
        const auto error_code_strerror = ::strerror_s(error.data(), error.size(), error_code);
    #else // POSIX and not GNU
        const auto error_code_strerror = ::strerror_r(error_code, error.data(), error.size());
    #endif // _WIN32 || WIN32

        return (OLD_GLIBC_FAILURE == error_code_strerror) ? errno : error_code_strerror;
#else // POSIX and GNU
        if (const auto* const error_message = ::strerror_r(error_code, error.data(), error.size());
            nullptr != error_message) {
            error = error_message;
            return ::SUCCESS;
        }

        return errno;
#endif // defined(_WIN32) || defined(WIN32) || not defined(__USE_GNU)
    }

    [[noreturn]] void panic(const std::string_view message, const std::string_view error_type) noexcept {
        static constexpr std::string_view SEPARATOR{": "};

        const auto error_type_is_blank = web_server::helpers::is_blank(error_type);

        const std::string_view valid_error_type{(error_type_is_blank ? "" : error_type)};
        const std::string_view error_type_separator{(error_type_is_blank ? "" : SEPARATOR)};

        const auto message_is_blank = web_server::helpers::is_blank(message);

        const std::string_view blank_message_error{(message_is_blank ? " (Blank panic message)" : "")};
        const std::string_view valid_message{(message_is_blank ? "" : message)};

        std::cerr << "Panics" << blank_message_error << SEPARATOR << valid_error_type << error_type_separator
                  << valid_message << '\n';

        std::terminate();
    }
} // namespace

namespace web_server::helpers {
    void panic(const std::string_view message) noexcept { ::panic(message, ""); }

    void panic_due_to_logic_error(const std::string_view message) noexcept { ::panic(message, "Logic error"); }

    std::string string_error(const errno_t error_code) noexcept {
        static constexpr errno_t SUCCESS{0};

        static constexpr size_type ERROR_BUFFER_SIZE{256};
        static constexpr size_type BUFFER_SIZE_MULTIPLIER{2};
        static constexpr size_type MAX_BUFFER_SIZE{std::numeric_limits<size_type>::max() / 2};

        static constexpr auto OUT_OF_RANGE_ERROR_BUFFER{static_cast<errno_t>(std::errc::result_out_of_range)};

        std::string error{};
        error.resize(ERROR_BUFFER_SIZE);

        if (auto error_code_strerror = ::safe_strerror(error, error_code); SUCCESS != error_code_strerror) {
            while (OUT_OF_RANGE_ERROR_BUFFER == error_code_strerror) {
                if (error.size() > MAX_BUFFER_SIZE) {
                    panic(fmt::format(
                        FMT_STRING("Out of memory, cannot get the error message linked to the error code '{:d}'."),
                        error_code));
                }

                error.resize(error.size() * BUFFER_SIZE_MULTIPLIER);

                error_code_strerror = ::safe_strerror(error, error_code);
            }

            panic(fmt::format(
                FMT_STRING("Cannot get the error message linked to the error code '{:d}', due to this error '{:d}'."),
                error_code,
                error_code_strerror));
        }

        return error;
    }
} // namespace web_server::helpers
