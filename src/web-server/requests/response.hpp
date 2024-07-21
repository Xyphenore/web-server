#ifndef REQUESTS_RESPONSE_HPP
#define REQUESTS_RESPONSE_HPP

#include <cerrno>
#include <cstring>
#include <system_error>
#include <string>
#include <fstream>
#include <ios>
#include <iostream>
#include <filesystem>
#include <utility>

#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include <asio/read.hpp>
#include <asio/buffer.hpp>

#include "version.hpp"
#include "status.hpp"
#include "request.hpp"
#include "errors.hpp"

namespace web_server::requests {
    namespace errors {
        constexpr static auto READ_ERROR_CODE = 10;

        class FileReadError final: public std::ios_base::failure {
            public:
                explicit FileReadError(const std::filesystem::path& file) :
                    std::ios_base::failure{
                        "Cannot read a line from the file: '"
                        + (((file.string() += "'. The reason: '") += std::strerror(errno)) += "'."),
                        std::error_code(READ_ERROR_CODE, std::iostream_category())
                    } {}
        };

        class CannotOpenFileError final: public std::ios_base::failure {
            public:
                explicit CannotOpenFileError(const std::filesystem::path& file) :
                    std::ios_base::failure{
                        "Cannot open the file: '"
                        + (((file.string() += "'. The reason: '") += std::strerror(errno)) += "'."),
                        std::error_code(READ_ERROR_CODE, std::iostream_category())
                    } {}
        };
    } // namespace errors

    /// HTTP response.
    ///
    /// # How to use it?
    ///
    /// ```rust
    /// use std::net::TcpListener;
    ///
    /// use crate::requests::{Request, Status};
    ///
    /// let listener = TcpListener::bind("127.0.0.1:8000").unwrap();
    ///
    /// for stream in listener.incoming() {
    ///     let request = Request::from_stream(stream.unwrap());
    ///
    ///     let response = request.make_response_with_status(Status::OK);
    ///     response.send();
    /// }
    /// ```
    class Response final {
        public:
            using TcpStream = asio::ip::tcp::socket;

            [[nodiscard]] static Response from(Request request, const Status& status) noexcept {
                return {
                    request.version(),
                    status,
                    request.take_stream(),
                };
            }

            /// Add the content of the file to the [`Response`].
            ///
            /// # Returns
            ///
            /// Returns the instance [`Response`], or [`std::io::Error`] if an error happened
            /// during the read of the file.
            ///
            /// # Examples
            ///
            /// ```rust
            /// use std::net::TcpListener;
            /// use std::path::Path;
            ///
            /// use crate::requests::{Request, Status};
            ///
            /// let listener = TcpListener::bind("127.0.0.1:8000").unwrap();
            ///
            /// for stream in listener.incoming() {
            ///     let request = Request::from_stream(stream.unwrap());
            ///
            ///     let response = request.make_response_with_status(Status::OK);
            ///     response.add_file(Path::new("file_to_be_loaded"));
            ///     response.send();
            /// }
            /// ```
            void add_file(const std::filesystem::path& filename) {
                std::ifstream file{filename};
                if (!file.good()) {
                    throw errors::CannotOpenFileError{filename};
                }

                std::string read_content{};

                while (!file.eof()) {
                    file.clear();

                    std::ignore = std::getline(file, read_content);
                    (contents_ += read_content) += '\n';

                    if (file.bad()) {
                        throw errors::FileReadError{filename};
                    }

                    if (file.fail() && (!file.eof())) {
                        if (read_content.size() != read_content.max_size()) {
                            throw errors::FileReadError{filename};
                        }

                        std::cerr << "Warning: The content string is full."
                            << " Need one more round to read the file content.\n";
                    }
                }
            }

            /// Send the response to the stream [`TcpStream`].
            ///
            /// # Examples
            ///
            /// ```rust
            /// use std::net::TcpListener;
            ///
            /// use crate::requests::{Request, Status};
            ///
            /// let listener = TcpListener::bind("127.0.0.1:8000").unwrap();
            ///
            /// for stream in listener.incoming() {
            ///     let request = Request::from_stream(stream.unwrap());
            ///
            ///     let response = request.make_response_with_status(Status::OK);
            ///     response.send();
            /// }
            /// ```
            ///
            /// # Panics
            ///
            /// - If the [`TcpStream::write_all`] panics.
            void send() {
                const auto message = to_string();

                if (const auto transfered_size = asio::write(stream_, asio::buffer(message));
                    message.size() != transfered_size) {
                    throw errors::MessageNotCompletelySendError{
                        message.size() - transfered_size,
                        stream_.remote_endpoint().address()
                    };
                }

                stream_.shutdown(TcpStream::shutdown_send);

                try {
                    std::string content{};
                    std::ignore = asio::read(stream_, asio::dynamic_buffer(content));
                }
                catch (const asio::system_error& error) {
                    if (error.code() != asio::error::eof) {
                        throw;
                    }
                }

                stream_.close();
            }

        private:
            Response(const Version version, const Status status, TcpStream stream) :
                version_{version}, status_{status}, stream_{std::move(stream)} {}

            [[nodiscard]] std::string to_string() const noexcept {
                auto value = version_.to_string();
                value += ' ';
                value += status_.to_string();
                value += "\r\n";

                value += "Content-Length: " + std::to_string(contents_.length());
                value += "\r\n\r\n";
                value += contents_;

                return value;
            }

            Version version_;
            Status status_;
            std::string contents_{};
            TcpStream stream_;
    };
} // namespace web_server::requests

#endif // REQUESTS_RESPONSE_HPP
