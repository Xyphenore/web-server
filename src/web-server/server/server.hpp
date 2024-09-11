#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include "debug.hpp"
#include "http_handlers.hpp"
#include "io_context.hpp"
#include "running_state.hpp"

#include <web-server/helpers/compat.hpp>
#include <web-server/helpers/sockets.hpp>
#include <web-server/requests/errors.hpp>
#include <web-server/requests/request.hpp>
#include <web-server/threads/pool.hpp>

#include <asio/error.hpp>
#include <asio/error_code.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>

/**
 * The namespace contaning the @ref WebServer.
 *
 * @section examples Examples
 *
 * @code{.cpp}
 * #include <iostream>
 * #include <string>
 *
 * #include <web-server/server.hpp>
 * #include <web-server/requests/method.hpp>
 * #include <web-server/tools.hpp>
 *
 * #include <web-server/routes/index.hpp>
 * #include <web-server/routes/slow_request.hpp>
 *
 * int main() {
 *     namespace ws = web_server;
 *     namespace requests = ws::requests;
 *     namespace routes = ws::routes;
 *     namespace tools = ws::tools;
 *
 *     try {
 *         auto server = ws::WebServer{tools::NonZeroSizeT(3), ws::Debug::from(false)};
 *         return server.add_listener(requests::Method::GET("/"), routes::index::get)
 *                      .add_listener(requests::Method::GET("/slow_request"), routes::slow_request::get)
 *                      .serve();
 *     }
 *     catch (const std::exception& error) {
 *         std::cerr << ((std::string{"Critical error"} += error.what()) += '\n');
 *     }
 * }
 * @endcode
 *
 * @see WebServer
 *
 * @since 0.1.0
 *
 * @author Xyphenore
 */
namespace web_server::server {
    constexpr auto DEFAULT_DEBUG{Debug::Off};

    /// The web server.
    ///
    /// # How to use it?
    ///
    /// ```rust
    /// use crate::server::{WebServer, Debug};
    /// use crate::requests::{Method, Status, Request, Response};
    ///
    /// fn process(request: Request) -> Response {
    ///     request.make_response_with_status(Status::OK)
    /// }
    ///
    /// let server = WebServer::new(5, Debug::False);
    /// server.add_listener(Method::get("/"), process);
    ///
    /// server.serve();
    /// ```
    template <threads::WorkerAmount amount = threads::DEFAULT_AMOUNT, Debug debug = DEFAULT_DEBUG>
    class WebServer final {
        public:
            using Method = HTTPHandlers::Method;
            using Handler = HTTPHandlers::Handler;

            using ExitCode = decltype(EXIT_FAILURE);

            /// Create the [`WebServer`].
            ///
            /// # Parameters
            ///
            /// - `amount_workers`: The number must be greater than 0, else panics.
            /// - `debug`: Activate the debug mode of the server, cf.[`Debug`].
            ///
            /// # Returns
            ///
            /// Returns a new instance of [`WebServer`].
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::server::{WebServer, Debug};
            ///
            /// let server = WebServer::new(5, Debug::False);
            /// ```
            ///
            /// # Panics
            ///
            /// - If `amount_workers` is equal to 0.
            WebServer() noexcept = default;

            /// Execute the server and process incoming requests on `127.0.0.1:8000`.
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::server::{WebServer, Debug};
            /// use crate::requests::{Method, Status, Request, Response};
            ///
            /// fn process(request: Request) -> Response {
            ///     request.make_response_with_status(Status::OK)
            /// }
            ///
            /// let server = WebServer::new(5, Debug::False);
            /// server.add_listener(Method::get("/"), process);
            ///
            /// server.serve();
            /// ```
            ///
            /// # Panics
            ///
            /// - If it is not possible to bind the [`TcpListener`] to `127.0.0.1:8000`,
            /// cf.[`TcpListener::bind()`].
            /// - If the [`TcpListener`] cannot enter into the non-blocking mode,
            /// cf.[`TcpListener::set_nonblocking()`].
            /// - If [`TcpListener::local_addr()`] fails.
            /// - If [`ctrlc::set_handler()`] fails.
            /// - If the state `is_running` cannot be locked.
            /// - If the incoming [`TcpStream`] fails.
            /// - If the incoming [`TcpStream`] cannot enter into the blocking mode.
            /// - If the process of the incoming stream, panics.
            [[nodiscard]] ExitCode serve();

            /// Add the route with the [`Method`] and the [`HTTPListener`].
            ///
            /// # Parameters
            ///
            /// - `method`: The [`Method`] to register.
            /// The `method` must not be yet registered, else panics.
            /// - `listener`: The function to process the incoming request for the `method`.
            ///
            /// # Returns
            ///
            /// Returns the mutable reference to the current instance of [`WebServer`].
            ///
            /// # Examples
            ///
            /// ```rust
            /// use crate::server::{WebServer, Debug};
            /// use crate::requests::{Method, Status, Request, Response};
            ///
            /// fn process(request: Request) -> Response {
            ///     request.make_response_with_status(Status::OK)
            /// }
            ///
            /// let server = WebServer::new(5, Debug::False);
            /// server.add_listener(Method::get("/"), process);
            /// ```
            ///
            /// # Panics
            ///
            /// - If the `method` is already registered.
            WebServer& link(Method method, Handler handler) noexcept;

        private:
            using Address = IOContext::Address;
            using BlockingMode = IOContext::BlockingMode;

            using WorkerPool = threads::WorkerPool<amount>;

            using Job = typename WorkerPool::Job;
            using Request = typename Job::Request;
            using Stream = typename Request::Stream;

            static constexpr auto HTTP_PORT{8000};

            /// Process the incoming [`TcpStream`].
            ///
            /// # Panics
            ///
            /// - If the creation of [`Request`] from the stream, panics.
            /// cf.[`Request::from_stream()`].
            /// - If the execution of the request, panics.
            /// cf.[`WorkerPool::execute()`].
            void handle(Stream stream);

            void execute(Request request) noexcept;

            HTTPHandlers handlers_;

            std::size_t cpt_{0};
            bool debug_{debug == Debug::On};

            IOContext io_context_;
            RunningState is_running_{io_context_};
            WorkerPool workers_;
    };
} // namespace web_server::server

namespace web_server {
    using Debug = server::Debug;
    using WorkerAmount = threads::WorkerAmount;

    constexpr auto DEFAULT_AMOUNT{threads::DEFAULT_AMOUNT};
    constexpr auto DEFAULT_DEBUG{server::DEFAULT_DEBUG};

    template <WorkerAmount amount = DEFAULT_AMOUNT, Debug debug = DEFAULT_DEBUG>
    using WebServer = server::WebServer<amount, debug>;
} // namespace web_server

namespace web_server::server {
    template <WorkerAmount amount, Debug debug>
    typename WebServer<amount, debug>::ExitCode WebServer<amount, debug>::serve() {
        auto listener = io_context_.listen(Address{asio::ip::tcp::v4(), HTTP_PORT}, BlockingMode::Off);

        is_running_ = true;

        asio::error_code error_code{};

        while (is_running_) {
            auto stream = io_context_.make_socket();
            listener.accept(stream, error_code);

            if (error_code) {
                if (asio::error::would_block != error_code) {
                    fmt::println(
                        std::cerr,
                        FMT_STRING("Error during the acceptation of a new connection: {}."),
                        error_code.message());
                }

                continue;
            }

            stream.non_blocking(false);

            try {
                this->handle(std::move(stream));
            }
            catch (const std::exception& error) {
                fmt::println(
                    std::cerr,
                    FMT_STRING("Error during the build of the request (from {}): {}."),
                    stream.remote_endpoint(),
                    error.what());
            }
        }

        return EXIT_SUCCESS;
    }

    template <WorkerAmount amount, Debug debug>
    WebServer<amount, debug>& WebServer<amount, debug>::link(Method method, Handler handler) noexcept {
        handlers_.link(std::move(method), std::move(handler));
        return *this;
    }

    template <WorkerAmount amount, Debug debug>
    void WebServer<amount, debug>::handle(Stream stream) {
        using requests::errors::MessagePartiallySentError;
        using requests::errors::ReceiveTooBigMessageError;

        try {
            this->execute(Request::from(std::move(stream)));
        }
        catch (const ReceiveTooBigMessageError& error) {
            fmt::println(
                std::cerr,
                FMT_STRING("Error during the building of the request from the stream: {}."),
                error.what());
        }
        catch (const MessagePartiallySentError& error) {
            fmt::println(std::cerr, FMT_STRING("Error during the error page sent: {}."), error.what());
        }
    }

    template <WorkerAmount amount, Debug debug>
    void WebServer<amount, debug>::execute(Request request) noexcept {
        if (debug_) {
            fmt::println(std::cout, "Request {:d}: {}", cpt_, request.method());
        }
        ++cpt_;

        auto listener = handlers_.linked_to(request.method());
        workers_.execute(Job{std::move(request), std::move(listener)});
    }
} // namespace web_server::server

#endif // SERVER_SERVER_HPP
