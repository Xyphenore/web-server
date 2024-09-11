#ifndef REQUESTS_RESPONSE_HPP
#define REQUESTS_RESPONSE_HPP

#include "request.hpp"
#include "status.hpp"

#include <web-server/helpers/sockets.hpp>

#include <filesystem>
#include <string>
#include <utility>

namespace web_server::requests {
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
    class [[nodiscard]] Response final {
        public:
            using Path = std::filesystem::path;

            using Request = Request;
            using Status = Status;
            using Version = Request::Version;
            using Stream = Request::Stream;

            static Response from(Request request, Status status) noexcept;

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
            void add_file(const Path& filename);

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
            void send();

        private:
            explicit Response(Version version, Status status, Stream&& stream) noexcept;

            friend void swap(Response& lhs, Response& rhs) noexcept {
                std::swap(lhs.status_, rhs.status_);
                std::swap(lhs.version_, rhs.version_);
                std::swap(lhs.contents_, rhs.contents_);
                std::swap(lhs.stream_, rhs.stream_);
            }

            Status status_;
            Version version_;
            std::string contents_;
            Stream stream_;
    };
} // namespace web_server::requests

namespace web_server::requests {
    inline Response::Response(const Version version, const Status status, Stream&& stream) noexcept:
    status_{status}, version_{version}, stream_{std::move(stream)} {}

    inline Response Response::from(Request request, const Status status) noexcept {
        return Response{
            request.version(),
            status,
            request.take_stream(),
        };
    }
} // namespace web_server::requests

#endif // REQUESTS_RESPONSE_HPP
