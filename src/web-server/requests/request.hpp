#ifndef REQUESTS_REQUEST_HPP
#define REQUESTS_REQUEST_HPP

#include <stdexcept>
#include <utility>

#include <asio/write.hpp>
#include <asio/read_until.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/address.hpp>

#include "method.hpp"
#include "version.hpp"
#include "status.hpp"
#include "errors.hpp"

namespace web_server::requests {
    namespace errors {
        class ReceiveMessageIsTooBigError final: public std::runtime_error {
            public:
                explicit ReceiveMessageIsTooBigError(const asio::ip::address& client_address):
                    std::runtime_error{
                        "Error: the request receives from the client ("
                        + (client_address.to_string() += ") is too big.\n")
                    } {}
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
    class Request final {
        public:
            using TcpStream = asio::ip::tcp::socket;

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
            [[nodiscard]] static Request from(TcpStream stream) {
                std::string content{};

                if (const auto read_size = asio::read_until(stream, asio::dynamic_buffer(content), "\r\n");
                    content.max_size() <= read_size) {
                    std::istringstream parts{content};

                    std::string unused{};
                    // Drop the method verb
                    std::ignore = std::getline(parts, unused, ' ');
                    // Drop the URI
                    std::ignore = std::getline(parts, unused, ' ');

                    std::string version{};
                    std::ignore = std::getline(parts, version, '\n');
                    version.resize(version.find_last_not_of("\r\n") + static_cast<std::size_t>(1));

                    auto error_response = ((version + ' ')
                                           += Status::UNPROCESSABLE_CONTENT.to_string())
                                          += "\r\n\r\n";

                    if (const auto transfered_size = asio::write(stream, asio::buffer(error_response));
                        error_response.size() > transfered_size) {
                        throw errors::MessageNotCompletelySendError{
                            error_response.size() - transfered_size,
                            stream.remote_endpoint().address()
                        };
                    }

                    throw errors::ReceiveMessageIsTooBigError{stream.remote_endpoint().address()};
                }

                std::istringstream content_stream{content};

                std::string method{};
                std::string part{};

                std::ignore = std::getline(content_stream, part, ' ');
                (method += part) += ' ';
                std::ignore = std::getline(content_stream, part, ' ');
                method += part;

                std::string version{};
                std::ignore = std::getline(content_stream, version, '\n');
                version.resize(version.find_last_not_of("\r\n") + static_cast<std::size_t>(1));

                return {
                    Method::from(method),
                    Version::from(version),
                    std::move(stream)
                };
            }

            [[nodiscard]] Method method() const noexcept {
                return method_;
            }

            [[nodiscard]] Version version() const noexcept {
                return version_;
            }

            TcpStream take_stream() noexcept {
                return std::move(stream_);
            }

        private:
            Request(Method method, const Version version, TcpStream stream) noexcept :
                method_{std::move(method)}, version_{version}, stream_{std::move(stream)} {}

            Method method_;
            Version version_;
            TcpStream stream_;
    };
} // namespace web_server::requests

#endif // REQUESTS_REQUEST_HPP
