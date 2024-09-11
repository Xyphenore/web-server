#ifndef REQUESTS_METHOD_HPP
#define REQUESTS_METHOD_HPP

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cstdint>
#include <iosfwd>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace web_server::requests {
    namespace errors {
        enum class [[nodiscard]] ElementTypes : std::uint8_t {
            Verb,
            URI,
        };

        std::ostream& operator<<(std::ostream& os, ElementTypes type) noexcept;

        namespace details {
            template <typename T>
            struct [[nodiscard]] HasStringConstructor final {
                private:
                    template <typename U>
                    static auto test(std::int32_t) -> decltype(U(std::string{}), std::true_type{});
                    template <typename>
                    static std::false_type test(...);

                public:
                    static constexpr bool value = std::is_same_v<decltype(test<T>(0)), std::true_type>;
            };

            template <typename T>
            constexpr bool HasStringConstructorValue = HasStringConstructor<T>::value;
        } // namespace details

        template <class Error>
        class [[nodiscard]] InvalidMethodElementError: public Error {
                static_assert(
                    std::is_base_of_v<std::logic_error, Error>,
                    "Error must be a subclass of std::logic_error.");
                static_assert(
                    details::HasStringConstructorValue<Error>,
                    "Error must have a constructor accepting a std::string.");

            protected:
                using ElementTypes = ElementTypes;

                explicit InvalidMethodElementError(std::string_view element, ElementTypes element_type) noexcept;
        };

        /// Indicate that [`Method::try_from()`] reads an invalid method verb.
        class [[nodiscard]] InvalidVerbError final: public InvalidMethodElementError<std::domain_error> {
            public:
                explicit InvalidVerbError(std::string_view verb) noexcept;
        };

        /// Indicate that [`Method::try_from()`] reads an invalid URI.
        class [[nodiscard]] InvalidURIError final: public InvalidMethodElementError<std::invalid_argument> {
            public:
                explicit InvalidURIError(std::string_view uri) noexcept;
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
    class [[nodiscard]] Method final {
        public:
            enum class [[nodiscard]] Verb : std::uint8_t {
                Get,
                Post,
                Update,
                Patch,
                Delete,
                Head,
                Options,
                Trace,
                Connect,
            };

            class [[nodiscard]] URI final {
                public:
                    static constexpr std::string_view REGEX_STRING{"(?:/[^ ]*)+"};

                    URI() noexcept = default;

                    explicit URI(std::string_view value);

                private:
                    friend std::ostream& operator<<(std::ostream& os, const URI& verb) noexcept {
                        return os << verb.value_;
                    }

                    friend void swap(URI& lhs, URI& rhs) noexcept { std::swap(lhs.value_, rhs.value_); }

                    [[nodiscard]] friend bool operator==(const URI& lhs, const URI& rhs) noexcept {
                        return lhs.value_ == rhs.value_;
                    }

                    [[nodiscard]] friend bool operator!=(const URI& lhs, const URI& rhs) noexcept {
                        return not(lhs == rhs);
                    }

                    friend std::hash<URI>;

                    std::string value_{"/"};
            };

            Method() noexcept = delete;

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
            explicit Method(Verb verb, URI uri) noexcept;

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
            static Method Get(URI uri) noexcept;
            static Method Get(std::string_view uri);

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
            static Method Post(URI uri) noexcept;
            static Method Post(std::string_view uri);

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
            static Method Patch(URI uri) noexcept;
            static Method Patch(std::string_view uri);

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
            static Method Update(URI uri) noexcept;
            static Method Update(std::string_view uri);

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
            static Method Delete(URI uri) noexcept;
            static Method Delete(std::string_view uri);

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
            static Method Head(URI uri) noexcept;
            static Method Head(std::string_view uri);

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
            static Method Options(URI uri) noexcept;
            static Method Options(std::string_view uri);

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
            static Method Trace(URI uri) noexcept;
            static Method Trace(std::string_view uri);

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
            static Method Connect(URI uri) noexcept;
            static Method Connect(std::string_view uri);

        private:
            [[nodiscard]] friend bool operator==(const Method& lhs, const Method& rhs) noexcept {
                return lhs.value_ == rhs.value_;
            }

            [[nodiscard]] friend bool operator!=(const Method& lhs, const Method& rhs) noexcept {
                return not(lhs == rhs);
            }

            friend std::ostream& operator<<(std::ostream& os, const Method& method) noexcept {
                return os << method.value_;
            }

            friend void swap(Method& lhs, Method& rhs) noexcept { std::swap(lhs.value_, rhs.value_); }

            friend std::hash<Method>;

            std::string value_;
    }; // namespace web_server::requests

    std::ostream& operator<<(std::ostream& os, Method::Verb verb) noexcept;

    Method::Verb to_verb(std::string_view verb);
    Method::URI to_uri(std::string_view uri);
} // namespace web_server::requests

template <>
struct std::hash<web_server::requests::Method> {
        std::size_t operator()(const web_server::requests::Method& method) const noexcept {
            return std::hash<std::string>{}(method.value_);
        }
};

template <>
struct fmt::formatter<web_server::requests::Method>: public fmt::ostream_formatter {};

template <>
struct fmt::formatter<web_server::requests::Method::Verb>: public fmt::ostream_formatter {};

template <>
struct fmt::formatter<web_server::requests::Method::URI>: public fmt::ostream_formatter {};

namespace web_server::requests {
    namespace errors {
        template <class Error>
        InvalidMethodElementError<Error>::InvalidMethodElementError(
            std::string_view element,
            ElementTypes element_type) noexcept:
        Error{fmt::format(FMT_STRING("Invalid Method {}: '{}'."), element_type, element)} {}

        inline InvalidVerbError::InvalidVerbError(const std::string_view verb) noexcept:
        InvalidMethodElementError{verb, ElementTypes::Verb} {}

        inline InvalidURIError::InvalidURIError(const std::string_view uri) noexcept:
        InvalidMethodElementError{uri, ElementTypes::URI} {}
    } // namespace errors

    inline Method::URI to_uri(const std::string_view uri) { return Method::URI{uri}; }

    inline Method Method::Get(URI uri) noexcept { return Method{Verb::Get, std::move(uri)}; }

    inline Method Method::Get(const std::string_view uri) { return Method::Get(URI{uri}); }

    inline Method Method::Post(URI uri) noexcept { return Method{Verb::Post, std::move(uri)}; }

    inline Method Method::Post(const std::string_view uri) { return Method::Post(URI{uri}); }

    inline Method Method::Patch(URI uri) noexcept { return Method{Verb::Patch, std::move(uri)}; }

    inline Method Method::Patch(const std::string_view uri) { return Method::Patch(URI{uri}); }

    inline Method Method::Update(URI uri) noexcept { return Method{Verb::Update, std::move(uri)}; }

    inline Method Method::Update(const std::string_view uri) { return Method::Update(URI{uri}); }

    inline Method Method::Delete(URI uri) noexcept { return Method{Verb::Delete, std::move(uri)}; }

    inline Method Method::Delete(const std::string_view uri) { return Method::Delete(URI{uri}); }

    inline Method Method::Head(URI uri) noexcept { return Method{Verb::Head, std::move(uri)}; }

    inline Method Method::Head(const std::string_view uri) { return Method::Head(URI{uri}); }

    inline Method Method::Options(URI uri) noexcept { return Method{Verb::Options, std::move(uri)}; }

    inline Method Method::Options(const std::string_view uri) { return Method::Options(URI{uri}); }

    inline Method Method::Trace(URI uri) noexcept { return Method{Verb::Trace, std::move(uri)}; }

    inline Method Method::Trace(const std::string_view uri) { return Method::Trace(URI{uri}); }

    inline Method Method::Connect(URI uri) noexcept { return Method{Verb::Connect, std::move(uri)}; }

    inline Method Method::Connect(const std::string_view uri) { return Method::Connect(URI{uri}); }
} // namespace web_server::requests

template <>
struct fmt::formatter<web_server::requests::errors::ElementTypes>: public fmt::ostream_formatter {};

template <>
struct std::hash<web_server::requests::Method::URI> final {
        std::size_t operator()(const web_server::requests::Method::URI& uri) const noexcept {
            return std::hash<std::string>{}(uri.value_);
        }
};

template <>
struct std::hash<web_server::requests::Method::Verb> final {
        std::size_t operator()(const web_server::requests::Method::Verb verb) const noexcept {
            return std::hash<std::underlying_type_t<web_server::requests::Method::Verb>>{}(fmt::enums::format_as(verb));
        }
};
#endif // REQUESTS_METHOD_HPP
