#ifndef REQUESTS_STATUS_HPP
#define REQUESTS_STATUS_HPP

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cstdint>
#include <iosfwd>

namespace web_server::requests {
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
    enum class [[nodiscard]] Status : std::uint16_t {
        /// HTTP status `OK`.
        ///
        /// [MDN - 200 OK](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/200)
        Ok = 200,

        /// HTTP status `NOT FOUND`.
        ///
        /// [MDN - 404 NOT FOUND](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/404)
        NotFound = 404,

        /// HTTP status `UNPROCESSABLE CONTENT`.
        ///
        /// [MDN - 422 UNPROCESSABLE CONTENT](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/422)
        UnprocessableContent = 422,
    };

    std::ostream& operator<<(std::ostream& os, Status status) noexcept;
} // namespace web_server::requests

template <>
struct fmt::formatter<web_server::requests::Status>: public fmt::ostream_formatter {};

#endif // REQUESTS_STATUS_HPP
