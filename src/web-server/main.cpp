#define ASIO_NO_DEPRECATED 1
#define ASIO_NO_TS_EXECUTORS 1

#include <iostream>
#include <string>

#include "server.hpp"
#include "requests/method.hpp"
#include "tools.hpp"

#include "routes/index.hpp"
#include "routes/slow_request.hpp"

namespace web_server {
    constexpr static auto DEBUG = false;
    constexpr static auto AMOUNT_WORKERS = static_cast<std::size_t>(3);
}

/**
 * Executable script to start the Web server.
 *
 * Add [`routes::index::get()`] and [`routes::slow_request::get()`] to the server.
 * The server listens on `127.0.0.1:8000`.
 *
 * @return int - The error code.
 */
int main() {
    namespace ws = web_server;
    namespace requests = ws::requests;
    namespace routes = ws::routes;
    namespace tools = ws::tools;

    try {
        auto server = ws::WebServer{tools::NonZeroSizeT(ws::AMOUNT_WORKERS), ws::Debug::from(ws::DEBUG)};
        return server.add_listener(requests::Method::GET("/"), routes::index::get)
                     .add_listener(requests::Method::GET("/slow_request"), routes::slow_request::get)
                     .serve();
    }
    catch (const std::exception& error) {
        std::cerr << ((std::string{"Critical error"} += error.what()) += '\n');
    }
}
