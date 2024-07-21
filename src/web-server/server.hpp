#ifndef WEB_SERVER_HPP
#define WEB_SERVER_HPP

#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <utility>

#include <asio/ip/tcp.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/basic_endpoint.hpp>
#include <asio/signal_set.hpp>
#include <asio/error_code.hpp>
#include <asio/error.hpp>

#include "tools.hpp"
#include "requests/job.hpp"
#include "requests/request.hpp"
#include "requests/response.hpp"
#include "requests/errors.hpp"
#include "requests/method.hpp"
#include "threads/pool.hpp"

namespace web_server {
    /// Indicate if the debug mode is activated on the [`WebServer`].
    class Debug final {
        public:
            const static Debug True;
            const static Debug False;

            /// Get the [`Debug`] enumeration value from the bool value.
            [[nodiscard]] static Debug from(const bool debug) noexcept {
                return debug ? Debug::True : Debug::False;
            }

            [[nodiscard]] explicit operator bool() const {
                return value_;
            }

        private:
            explicit Debug(const bool value):
                value_{value} {}

            bool value_ = false;
    };

    const inline Debug Debug::True{true};
    const inline Debug Debug::False{false};

    namespace errors {
        class MethodAlreadyRegisteredError final: public std::invalid_argument {
            public:
                explicit MethodAlreadyRegisteredError(const requests::Method& method):
                    std::invalid_argument{
                        "The method " + method.to_string() += " has already a listener registered."
                    } {}
        };
    } //namespace errors

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
    class WebServer final {
        public:
            using TcpStream = asio::ip::tcp::socket;

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
            WebServer(const tools::NonZeroSizeT amount_workers, const Debug debug) :
                debug_{debug}, workers_{amount_workers} {}

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
            [[nodiscard]] int32_t serve() {
                namespace ip = asio::ip;
                using tcp = ip::tcp;

                tcp::acceptor listener{
                    io_context_,
                    tcp::endpoint(tcp::v4(), static_cast<ip::port_type>(8000))
                };
                listener.non_blocking(true);

                is_running_ = true;

                const auto local_endpoint = listener.local_endpoint();
                std::cout << ("Server started and waiting for incoming connections on '"
                              + ((local_endpoint.address().to_string() += ":")
                                 += std::to_string(static_cast<int32_t>(local_endpoint.port())))
                              += "'.\n");

                asio::signal_set signals{io_context_, SIGINT, SIGTERM, SIGABRT};
                signals.async_wait(
                    [this](const asio::error_code& error, const int32_t code) {
                        this->shutdown(error, code);
                    });

                std::thread io_thread{[this] { std::ignore = this->io_context_.run(); }};

                auto is_running = is_running_;
                asio::error_code error_code{};

                while (is_running) {
                    TcpStream socket(io_context_);
                    listener.accept(socket, error_code);

                    if (error_code) {
                        if (error_code != asio::error::would_block) {
                            std::cerr << ("Error during the acceptation of a new connection: "
                                          + (error_code.message() += '\n'));
                        }
                    }
                    else {
                        socket.non_blocking(false);
                        handle(std::move(socket));
                    }

                    {
                        const std::lock_guard lk{is_running_mutex_};
                        is_running = is_running_;
                    }
                }

                workers_.close();
                io_context_.stop();

                io_thread.join();

                return EXIT_SUCCESS;
            }

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
            WebServer& add_listener(const requests::Method& method, const requests::HTTPListener& listener) {
                if (const auto [iterator, inserted] =
                        listeners_.try_emplace(method, listener);
                    !inserted) {
                    throw errors::MethodAlreadyRegisteredError{method};
                }

                return *this;
            }

        private:
            /// Process the incoming [`Request`] if any listener is registered for the
            /// [`Method`] contained in the [`Request`].
            ///
            /// # Returns
            ///
            /// Returns the response `404 Not Found` with the `templates/not_found.html`
            /// content.
            ///
            /// # Panics
            ///
            /// - If [`Response::add_file()`] returns an error.
            [[nodiscard]] static requests::Response not_found_handler(requests::Request request) {
                auto response = requests::Response::from(std::move(request), requests::Status::NOT_FOUND);
                response.add_file(tools::TEMPLATES_DIRECTORY / "not_found.html");

                return response;
            }

            /// Process the incoming [`TcpStream`].
            ///
            /// # Panics
            ///
            /// - If the creation of [`Request`] from the stream, panics.
            /// cf.[`Request::from_stream()`].
            /// - If the execution of the request, panics.
            /// cf.[`WorkerPool::execute()`].
            void handle(TcpStream stream) {
                try {
                    execute_request(requests::Request::from(std::move(stream)));
                }
                catch (const requests::errors::ReceiveMessageIsTooBigError& error) {
                    std::cerr << ((std::string("Error during the building of the request from the stream: ")
                                   += error.what()) += '\n');
                }
                catch (const requests::errors::MessageNotCompletelySendError& error) {
                    std::cerr << ((std::string("Error during the error page send: ") += error.what()) += '\n');
                }
            }

            void execute_request(requests::Request request) {
                if (debug_) {
                    std::cout << ("Request "
                                  + ((std::to_string(cpt_) += ": ") += request.method().to_string())
                                  += '\n');
                }
                cpt_ += static_cast<std::size_t>(1);

                const auto listener = get_listener(request.method());
                workers_.execute(requests::Job(std::move(request), listener));
            }

            void shutdown(const asio::error_code& error, const int32_t signal_code) noexcept {
                if (asio::error::operation_aborted != error) {
                    std::cout << ("Receive the signal "
                                  + (std::to_string(signal_code)
                                     += ". Start the procedure of shutdown the server.\n"));

                    const std::lock_guard lk{is_running_mutex_};
                    is_running_ = false;
                }
            }

            [[nodiscard]] requests::HTTPListener get_listener(const requests::Method& method) const noexcept {
                try {
                    return listeners_.at(method);
                }
                catch (const std::out_of_range&) {
                    return web_server::WebServer::not_found_handler;
                }
            }

            asio::io_context io_context_{};
            std::size_t cpt_{};
            bool debug_ = false;
            std::unordered_map<requests::Method, requests::HTTPListener> listeners_{};
            threads::WorkerPool workers_{};

            bool is_running_ = false;
            std::mutex is_running_mutex_{};
    };
}

#endif // WEB_SERVER_HPP
