#ifndef REQUESTS_STATUS_HPP
#define REQUESTS_STATUS_HPP

#include <algorithm>
#include <execution>
#include <array>
#include <string>
#include <stdexcept>

namespace web_server::requests {
    namespace errors {
        class UnknownStatusError final: public std::domain_error {
            public:
                explicit UnknownStatusError(const int16_t code) noexcept:
                    std::domain_error{"Unknown status '" + (std::to_string(code) += "'.")} {}
        };

        class NotAllowedStatusCodeError final: public std::invalid_argument {
            public:
                explicit NotAllowedStatusCodeError(const int16_t code):
                    std::invalid_argument{"Invalid status code: '" + (std::to_string(code) += "'.")} {}
        };
    } // namespace errors

    /// HTTP status [MDN Status](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status).
    ///
    /// # How to use it?
    ///
    /// ```rust
    /// use crate::requests::Status;
    ///
    /// let ok = Status::Ok;
    /// let not_found = Status::NotFound;
    ///
    /// // You use it like an enumeration.
    /// ```
    class Status final {
        public:
            Status() = delete;

            explicit Status(const int16_t code) :
                code_{code} {
                Status::check_code(code_);
            }

            /// HTTP status `OK`.
            ///
            /// [MDN - 200 OK](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/200)
            const static Status OK;

            /// HTTP status `NOT FOUND`.
            ///
            /// [MDN - 404 NOT FOUND](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/404)
            const static Status NOT_FOUND;

            /// HTTP status `UNPROCESSABLE CONTENT`.
            ///
            /// [MDN - 422 UNPROCESSABLE CONTENT](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/422)
            const static Status UNPROCESSABLE_CONTENT;

            [[nodiscard]] std::string to_string() const {
                std::string value{};

                switch (code_) {
                    case OK_CODE: value = "OK";
                        break;
                    case NOT_FOUND_CODE: value = "NOT FOUND";
                        break;
                    case UNPROCESSABLE_CONTENT_CODE: value = "UNPROCESSABLE CONTENT";
                        break;
                    default: break;
                }

                if (value.empty()) {
                    throw errors::UnknownStatusError{code_};
                }

                return (std::to_string(code_) += " ") += value;
            }

        private:
            constexpr static int16_t OK_CODE = 200;
            constexpr static int16_t NOT_FOUND_CODE = 404;
            constexpr static int16_t UNPROCESSABLE_CONTENT_CODE = 422;

            constexpr static auto ALLOWED_CODES = std::array{
                OK_CODE, NOT_FOUND_CODE, UNPROCESSABLE_CONTENT_CODE,
            };

            static void check_code(const int16_t code) {
                if (const auto* const found_code = std::find(
                        std::execution::seq,
                        ALLOWED_CODES.cbegin(),
                        ALLOWED_CODES.cend(),
                        code
                        );
                    ALLOWED_CODES.cend() == found_code) {
                    throw errors::NotAllowedStatusCodeError{code};
                }
            }

            [[nodiscard]] friend bool operator==(const Status& lhs, const Status& rhs) noexcept {
                return lhs.code_ == rhs.code_;
            }

            int16_t code_;
    };

    const inline Status Status::OK{Status::OK_CODE};
    const inline Status Status::NOT_FOUND{Status::NOT_FOUND_CODE};
    const inline Status Status::UNPROCESSABLE_CONTENT{Status::UNPROCESSABLE_CONTENT_CODE};
} // namespace web_server::requests


template <>
struct std::hash<web_server::requests::Status> {
    [[nodiscard]] std::size_t operator()(const web_server::requests::Status& status) const noexcept {
        return std::hash<std::string>{}(status.to_string());
    }
};

#endif // REQUESTS_STATUS_HPP
