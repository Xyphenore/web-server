#ifndef ROUTES_INDEX_HPP
#define ROUTES_INDEX_HPP

#include <web-server/requests/response.hpp>
#include <web-server/requests/request.hpp>
#include <web-server/requests/status.hpp>
#include <web-server/tools.hpp>

/// The module contains all functions that process requests for the URI `/`.
///
/// # Examples
///
/// Check examples of [`WebServer::add_listener()`][add_listener],
/// to see how to add a route to the server.
///
/// <!-- References -->
///
/// [add_listener]: crate::server::WebServer::add_listener()
namespace web_server::routes::index {
    /// Process the `GET /`.
    ///
    /// # Returns
    ///
    /// Returns the response to send with [`Response::send()`], with the HTML page
    /// [`templates/index.html`](/templates/index.html).
    ///
    /// # Panics
    ///
    /// If the method [`Response::add_file()`] returns an error when adding a file.
    ///
    /// # Examples
    ///
    /// Check examples of [`WebServer::add_listener()`][add_listener],
    /// to see how to add the function to process the `GET /`.
    ///
    /// <!-- References -->
    ///
    /// [add_listener]: crate::server::WebServer::add_listener()
    [[nodiscard]] inline requests::Response get(requests::Request request) {
        auto response = requests::Response::from(std::move(request), requests::Status::OK);
        response.add_file(tools::TEMPLATES_DIRECTORY / "index.html");

        return response;
    }
} // namespace web_server::routes::index

#endif // ROUTES_INDEX_HPP
