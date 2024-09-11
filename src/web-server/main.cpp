#define ASIO_NO_DEPRECATED
#define ASIO_NO_TS_EXECUTORS
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#include "requests/method.hpp"

#include "routes/index.hpp"
#include "routes/slow_request.hpp"

#include "server/server.hpp"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <iostream>

/**
 * Executable script to start the Web server.
 *
 * Add @ref routes::index::get() and @ref routes::slow_request::get() to the server.
 * The server listens on `127.0.0.1:8000`.
 *
 * @returns EXIT_SUCCESS if the server is done.
 * @returns EXIT_FAILURE if the server has an error.
 *
 * @warning Don't accept any argument from CLI.
 * @note Print to stderr if an error has occurred.
 *
 * @see WebServer
 *
 * @since 0.1.0
 * @author Xyphenore
 */
int main() {
    using web_server::WebServer;
    using web_server::WorkerAmount;
    using web_server::requests::Method;

    namespace routes = web_server::routes;

    try {
        WebServer<WorkerAmount{3}> server{};
        return server.link(Method::Get("/"), routes::index::get)
            .link(Method::Get("/slow_request"), routes::slow_request::get)
            .serve();
    }
    catch (const std::exception& error) {
        fmt::println(std::cerr, FMT_STRING("Critical error: '{}'."), error.what());
    }
    catch (...) {
        // Ensure destructors are called.
        throw;
    }

    return EXIT_FAILURE;
}
