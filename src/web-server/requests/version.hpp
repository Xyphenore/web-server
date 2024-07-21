#ifndef REQUESTS_VERSION_HPP
#define REQUESTS_VERSION_HPP

#include <algorithm>
#include <execution>
#include <array>
#include <string>
#include <string_view>
#include <stdexcept>

#include <web-server/tools.hpp>

namespace web_server::requests {
    namespace errors {
        class InvalidHTTPVersionError final: public std::invalid_argument {
            public:
                explicit InvalidHTTPVersionError(std::string version):
                    std::invalid_argument{"Invalid HTTP version: '" + (std::move(version) += "'.")} {}
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
    class Version final {
        public:
            Version() = delete;

            /// HTTP Version 1
            ///
            /// [MDN - HTTP1](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http1.0_%E2%80%93_building_extensibility)
            const static Version HTTP_1;

            /// HTTP Version 1.1
            ///
            /// [MDN - HTTP1.1](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http1.1_%E2%80%93_the_standardized_protocol)
            const static Version HTTP_1_1;

            /// HTTP Version 2
            ///
            /// [MDN - HTTP2](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http2_%E2%80%93_a_protocol_for_greater_performance)
            const static Version HTTP_2;

            /// HTTP Version 3
            ///
            /// [MDN - HTTP3](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http3_-_http_over_quic)
            const static Version HTTP_3;

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
            [[nodiscard]] static Version from(const std::string_view value) {
                const auto upper_version = tools::to_uppercase(value);

                const static std::array ALLOWED_VERSIONS{
                    HTTP_1,
                    HTTP_1_1,
                    HTTP_2,
                    HTTP_3
                };

                const auto* const found_version = std::find_if(
                    std::execution::seq,
                    ALLOWED_VERSIONS.cbegin(),
                    ALLOWED_VERSIONS.cend(),
                    [&upper_version](const Version& version) {
                        return version.to_string() == upper_version;
                    }
                    );

                if (found_version == ALLOWED_VERSIONS.cend()) {
                    throw errors::InvalidHTTPVersionError(std::string{value});
                }

                return *found_version;
            }

            [[nodiscard]] std::string to_string() const noexcept {
                std::string value{"HTTP/"};
                value += major_;

                if (minor_ != '0') {
                    (value += '.') += minor_;
                }

                return value;
            }

        private:
            constexpr static auto DEFAULT_MINOR = '0';

            constexpr explicit Version(const char major) noexcept:
                major_{major} {}

            constexpr Version(const char major, const char minor) noexcept:
                major_{major}, minor_{minor} {}

            [[nodiscard]] friend bool operator==(const Version& lhs, const Version& rhs) noexcept {
                return (lhs.major_ == rhs.major_) && (lhs.minor_ == rhs.minor_);
            }

            char major_;
            char minor_ = DEFAULT_MINOR;
    };

    const inline Version Version::HTTP_1{'1'};
    const inline Version Version::HTTP_1_1{'1', '1'};
    const inline Version Version::HTTP_2{'2'};
    const inline Version Version::HTTP_3{'3'};
} // namespace web_server::requests

template <>
struct std::hash<web_server::requests::Version> {
    std::size_t operator()(const web_server::requests::Version version) const noexcept {
        return std::hash<std::string>{}(version.to_string());
    }
};

#endif // REQUESTS_VERSION_HPP
