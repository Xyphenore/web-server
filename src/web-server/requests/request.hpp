#ifndef REQUESTS_REQUEST_HPP
#define REQUESTS_REQUEST_HPP

#include "method.hpp"
#include "version.hpp"

#include <web-server/helpers/sockets.hpp>

#include <stdexcept>
#include <utility>

namespace web_server::requests {
    namespace errors {
        class [[nodiscard]] ReceiveTooBigMessageError final: public std::length_error {
            public:
                using Address = helpers::RemoteAddress;

                explicit ReceiveTooBigMessageError(const Address& client_address) noexcept;
        };

        class [[nodiscard]] InvalidHTTPRequestError final: public std::invalid_argument {
            public:
                explicit InvalidHTTPRequestError(std::string_view line) noexcept;
        };
    } // namespace errors

    /// HTTP request.
    ///
    /// # How to create it?
    ///
    /// ```rust
    /// use std::net::TcpListener;
    ///
    /// use crate::requests::Request;
    ///
    /// let listener = TcpListener::bind("127.0.0.1:8000").unwrap();
    ///
    /// for stream in listener.incoming() {
    ///     let request = Request::from(stream.unwrap());
    ///
    ///     // Process the request after.
    /// }
    /// ```
    class [[nodiscard]] Request final {
        public:
            using Stream = helpers::TCPStream;
            using Method = Method;
            using Version = Version;

            /// Create a [`Request`] from a [`TcpStream`].
            ///
            /// # Returns
            ///
            /// Returns a new instance of [`Request`].
            ///
            /// # Panics
            ///
            /// - If the read of `stream` fails.
            /// - If the read of the method from the `stream`.
            /// - If the read of the version from the `stream`.
            static Request from(Stream&& stream);

            Request() noexcept = delete;

            [[nodiscard]] Method method() const noexcept;

            [[nodiscard]] Version version() const noexcept;

            [[nodiscard]] Stream&& take_stream() noexcept;

        private:
            explicit Request(Method method, Version version, Stream&& stream) noexcept;

            friend void swap(Request& lhs, Request& rhs) noexcept {
                std::swap(lhs.method_, rhs.method_);
                std::swap(lhs.version_, rhs.version_);
                std::swap(lhs.stream_, rhs.stream_);
            }

            Method method_;
            Version version_;
            Stream stream_;
    };
} // namespace web_server::requests

namespace web_server::requests {
    inline Request::Stream&& Request::take_stream() noexcept { return std::move(stream_); }

    inline Request::Version Request::version() const noexcept { return version_; }

    inline Request::Method Request::method() const noexcept { return method_; }

    inline Request::Request(Method method, const Version version, Stream&& stream) noexcept:
    method_{std::move(method)}, version_{version}, stream_{std::move(stream)} {}
} // namespace web_server::requests

#endif // REQUESTS_REQUEST_HPP
