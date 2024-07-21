#ifndef REQUESTS_METHOD_HPP
#define REQUESTS_METHOD_HPP

#include <algorithm>
#include <execution>
#include <array>
#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>

#include <web-server/tools.hpp>

namespace web_server::requests {
    namespace errors {
        class InvalidMethodError: public std::invalid_argument {
            public:
                using invalid_argument::invalid_argument;
        };

        /// Indicate that [`Method::try_from()`] reads an invalid method verb.
        class InvalidVerbError final: public InvalidMethodError {
            public:
                explicit InvalidVerbError(std::string verb) noexcept:
                    InvalidMethodError{"Invalid Method verb: '" + (verb += "'.")} {}
        };

        /// Indicate that [`Method::try_from()`] reads an invalid URI.
        class InvalidURIError final: public InvalidMethodError {
            public:
                explicit InvalidURIError(std::string uri) noexcept:
                    InvalidMethodError{"Invalid Method URI: '" + (uri += "'.")} {}
        };
    } // namespace errors

    /// HTTP method
    ///
    /// [MDN - Methods](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods)
    ///
    /// # How to use it?
    ///
    /// # Class method
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let get_index = Method::get("/");
    /// let post_index = Method::post("/");
    /// let update_index = Method::update("/");
    /// let delete_index = Method::delete("/");
    /// let options_index = Method::options("/");
    /// let head_index = Method::head("/");
    /// let patch_index = Method::patch("/");
    /// let trace_index = Method::trace("/");
    /// let connect_index = Method::connect("/");
    /// ```
    ///
    /// # From a string
    ///
    /// ```
    /// use crate::requests::Method;
    ///
    /// let method = Method::try_from("GET /");
    /// assert!(method.is_ok());
    /// if let Ok(method) = method {
    ///     assert_eq!(method.to_string(), "GET /");
    /// }
    /// ```
    ///
    /// ```
    /// use crate::requests::Method;
    ///
    /// let method = Method::try_from("NOT /");
    /// assert!(method.is_err());
    /// ```
    ///
    /// ```
    /// use crate::requests::Method;
    ///
    /// let method = Method::from_str("GET /");
    /// assert!(method.is_ok());
    /// if let Ok(method) = method {
    ///     assert_eq!(method.to_string(), "GET /");
    /// }
    /// ```
    ///
    /// ```
    /// use crate::requests::Method;
    ///
    /// let method = Method::from_str("NOT /");
    /// assert!(method.is_err());
    /// ```
    class Method final {
        public:
            Method() = delete;

            /// Create a `GET` method with the URI.
            ///
            /// # Parameters
            ///
            /// - `uri`: The URI must be not blank.
            /// A [`String`] or [`&str`].
            ///
            /// # Returns
            ///
            /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::requests::Method;
            ///
            /// let get_index = Method::get("/");
            /// ```
            static Method GET(std::string uri) {
                return {"GET", std::move(uri)};
            }

            /// Create a `POST` method with the URI.
            ///
            /// # Parameters
            ///
            /// - `uri`: The URI must be not blank.
            /// A [`String`] or [`&str`].
            ///
            /// # Returns
            ///
            /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::requests::Method;
            ///
            /// let post_index = Method::post("/");
            /// ```
            static Method POST(std::string uri) {
                return {"POST", std::move(uri)};
            }

            /// Create a `PATCH` method with the URI.
            ///
            /// # Parameters
            ///
            /// - `uri`: The URI must be not blank.
            /// A [`String`] or [`&str`].
            ///
            /// # Returns
            ///
            /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::requests::Method;
            ///
            /// let patch_index = Method::patch("/");
            /// ```
            static Method PATCH(std::string uri) {
                return {"PATCH", std::move(uri)};
            }

            /// Create an `UPDATE` method with the URI.
            ///
            /// # Parameters
            ///
            /// - `uri`: The URI must be not blank.
            /// A [`String`] or [`&str`].
            ///
            /// # Returns
            ///
            /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::requests::Method;
            ///
            /// let update_index = Method::update("/");
            /// ```
            static Method UPDATE(std::string uri) {
                return {"UPDATE", std::move(uri)};
            }

            /// Create a `DELETE` method with the URI.
            ///
            /// # Parameters
            ///
            /// - `uri`: The URI must be not blank.
            /// A [`String`] or [`&str`].
            ///
            /// # Returns
            ///
            /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::requests::Method;
            ///
            /// let delete_index = Method::delete("/");
            /// ```
            static Method DELETE_(std::string uri) {
                return {"DELETE", std::move(uri)};
            }

            /// Create a `HEAD` method with the URI.
            ///
            /// # Parameters
            ///
            /// - `uri`: The URI must be not blank.
            /// A [`String`] or [`&str`].
            ///
            /// # Returns
            ///
            /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::requests::Method;
            ///
            /// let head_index = Method::head("/");
            /// ```
            static Method HEAD(std::string uri) {
                return {"HEAD", std::move(uri)};
            }

            /// Create an `OPTIONS` method with the URI.
            ///
            /// # Parameters
            ///
            /// - `uri`: The URI must be not blank.
            /// A [`String`] or [`&str`].
            ///
            /// # Returns
            ///
            /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::requests::Method;
            ///
            /// let options_index = Method::options("/");
            /// ```
            static Method OPTIONS(std::string uri) {
                return {"OPTIONS", std::move(uri)};
            }

            /// Create a `TRACE` method with the URI.
            ///
            /// # Parameters
            ///
            /// - `uri`: The URI must be not blank.
            /// A [`String`] or [`&str`].
            ///
            /// # Returns
            ///
            /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::requests::Method;
            ///
            /// let trace_index = Method::trace("/");
            /// ```
            static Method TRACE(std::string uri) {
                return {"TRACE", std::move(uri)};
            }

            /// Create a `CONNECT` method with the URI.
            ///
            /// # Parameters
            ///
            /// - `uri`: The URI must be not blank.
            /// A [`String`] or [`&str`].
            ///
            /// # Returns
            ///
            /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::requests::Method;
            ///
            /// let connect_index = Method::connect("/");
            /// ```
            static Method CONNECT(std::string uri) {
                return {"CONNECT", std::move(uri)};
            }

            /// Create a new instance of [`Method`] from the line.
            ///
            /// # Parameters
            ///
            /// - `line`: The line must start like: `METHOD URI`.
            /// A [`String`] or [`&str`].
            ///
            /// # Returns
            ///
            /// Returns the instance of [`Method`], or the error if `line` is invalid.
            [[nodiscard]] static Method from(const std::string& line) {
                std::istringstream line_stream{line};

                std::string verb{};
                std::ignore = std::getline(line_stream, verb, ' ');

                std::string uri{};
                std::ignore = std::getline(line_stream, uri, ' ');

                return {std::move(verb), std::move(uri)};
            }

            [[nodiscard]] std::string to_string() const noexcept {
                return verb_ + "_" += uri_;
            }

        private:
            /// Build a [`Method`] with the `method` and `uri`.
            ///
            /// # Parameters
            ///
            /// - `method`: A value in [`Method::ALLOWED_METHODS`].
            /// - `uri`: The URI of the method.
            /// A [`String`] or [`&str`].
            ///
            /// # Returns
            ///
            /// Returns a new instance of [`Method`] if all is good or an [`InvalidMethodError`]
            /// if the `uri` is blank.
            ///
            /// # Panics
            ///
            /// If `method` is not in [`Method::ALLOWED_METHODS`].
            Method(std::string verb, std::string uri) :
                verb_{std::move(verb)}, uri_{std::move(uri)} {
                if (tools::is_blank(uri_)) {
                    throw errors::InvalidURIError{uri_};
                }

                if (const auto* const* const found_method = std::find(
                        std::execution::seq,
                        ALLOWED_METHODS.cbegin(),
                        ALLOWED_METHODS.cend(),
                        tools::to_uppercase(verb_)
                        );
                    found_method == ALLOWED_METHODS.cend()) {
                    throw errors::InvalidVerbError{verb_};
                }
            }

            [[nodiscard]] friend bool operator==(const Method& lhs, const Method& rhs) noexcept {
                return lhs.to_string() == rhs.to_string();
            }

            constexpr static auto ALLOWED_METHODS = std::array{
                "GET", "POST", "UPDATE", "PATCH", "DELETE", "HEAD", "OPTIONS", "TRACE", "CONNECT",
            };

            std::string verb_;
            std::string uri_;
    };
} // namespace web_server::requests


template <>
struct std::hash<web_server::requests::Method> {
    std::size_t operator()(const web_server::requests::Method& method) const noexcept {
        return std::hash<std::string>{}(method.to_string());
    }
};

#endif // REQUESTS_METHOD_HPP
