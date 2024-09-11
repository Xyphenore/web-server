#ifndef REQUESTS_VERSION_HPP
#define REQUESTS_VERSION_HPP

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cstdint>
#include <iosfwd>
#include <stdexcept>
#include <string_view>

namespace web_server::requests {
    namespace errors {
        class [[nodiscard]] InvalidHTTPVersionError final: public std::invalid_argument {
            public:
                explicit InvalidHTTPVersionError(std::string_view version);
        };
    } // namespace errors

    /// HTTP version.
    ///
    /// # How to use it?
    ///
    /// ```rust
    /// use crate::requests::Version;
    ///
    /// let version_1 = Version::Http1;
    /// let version_1_1 = Version::Http1_1;
    /// let version_2 = Version::Http2;
    /// let version_3 = Version::Http3;
    ///
    /// // You can use it like an enumeration.
    /// ```
    ///
    /// ```rust
    /// use crate::requests::Version;
    ///
    /// let line = "HTTP/2";
    ///
    /// let version = Version::from(line);
    /// ```
    enum class [[nodiscard]] Version : std::uint8_t {
        // Default version.
        /// HTTP Version 1.1
        ///
        /// [MDN -
        /// HTTP1.1](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http1.1_%E2%80%93_the_standardized_protocol)
        HTTP_1_1,

        /// HTTP Version 1
        ///
        /// [MDN -
        /// HTTP1](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http1.0_%E2%80%93_building_extensibility)
        HTTP_1,

        /// HTTP Version 2
        ///
        /// [MDN -
        /// HTTP2](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http2_%E2%80%93_a_protocol_for_greater_performance)
        HTTP_2,

        /// HTTP Version 3
        ///
        /// [MDN -
        /// HTTP3](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http3_-_http_over_quic)
        HTTP_3,
    };

    std::ostream& operator<<(std::ostream& os, Version version) noexcept;

    /// Get a constant reference to an HTTP version.
    ///
    /// # Parameters
    ///
    /// - `s`: The line must be in the form: `HTTP/{version}`.
    /// `{version}` can be like `major` or `major.minor`.
    ///
    /// # Returns
    ///
    /// Returns the constant to the good [`Version`], or a
    /// [`InvalidHTTPVersionError`] if `line` does not respect the pattern.
    ///
    /// # Examples
    ///
    /// ```rust
    /// let version = Version::from_str("HTTP/1");
    /// assert!(version.is_ok());
    ///
    /// if let Some(version) = version {
    ///     assert_eq!(version.to_string(), "HTTP/1");
    /// }
    /// ```
    ///
    /// ```rust
    /// let version = Version::from_str("1.0");
    /// assert!(version.is_err());
    ///
    /// if let Err(error) = version {
    ///     assert_eq!(error, "Invalid HTTP version: '1.0'");
    /// }
    /// ```
    Version to_version(std::string_view version);
} // namespace web_server::requests

template <>
struct fmt::formatter<web_server::requests::Version>: public fmt::ostream_formatter {};

#endif // REQUESTS_VERSION_HPP
