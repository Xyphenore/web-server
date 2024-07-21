#ifndef ROUTES_SLOW_REQUEST_HPP
#define ROUTES_SLOW_REQUEST_HPP

#include <thread>
#include <chrono>

#include <web-server/requests/response.hpp>
#include <web-server/requests/request.hpp>
#include <web-server/requests/status.hpp>
#include <web-server/tools.hpp>

/// The module contains all functions that process requests for the URI `/slow_request`.
///
/// # Examples
///
/// Check examples of [`WebServer::add_listener()`][add_listener],
/// to see how to add a route to the server.
///
/// <!-- References -->
///
/// [add_listener]: crate::server::WebServer::add_listener()
namespace web_server::routes::slow_request {
    constexpr static std::chrono::seconds SLEEP_TIME{5};

    /// Process the `GET /slow_request`.
    ///
    /// The function sleeps 5 secondes before returning the response, to simulate a slow
    /// request to process.
    ///
    /// # Returns
    ///
    /// Returns the response to send with [`Response::send()`], with the HTML page
    /// [`templates/slow_request.html`](/templates/slow_request.html).
    ///
    /// # Panics
    ///
    /// If the method [`Response::add_file()`] returns an error when adding a file.
    ///
    /// # Examples
    ///
    /// Check examples of [`WebServer::add_listener()`][add_listener],
    /// to see how to add the function to process the `GET /slow_request`.
    ///
    /// <!-- References -->
    ///
    /// [add_listener]: crate::server::WebServer::add_listener()
    [[nodiscard]] inline requests::Response get(requests::Request request) {
        auto response = requests::Response::from(std::move(request), requests::Status::OK);
        response.add_file(tools::TEMPLATES_DIRECTORY / "slow_request.html");

        std::this_thread::sleep_for(SLEEP_TIME);

        return response;
    }
} // namespace web_server::routes::slow_request

#endif // ROUTES_SLOW_REQUEST_HPP
