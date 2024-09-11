#ifndef ROUTES_INDEX_HPP
#define ROUTES_INDEX_HPP

#include <web-server/requests/request.hpp>
#include <web-server/requests/response.hpp>

/**
 * The module contains all functions that process requests for the URI `/`.
 *
 * # Examples
 *
 * Check examples of @ref WebServer::add_listener(), to see how to add a route to the server.
 */
namespace web_server::routes::index {
    /**
     * Process the `GET /`.
     *
     * @returns Response - The reponse with the HTML page @ref `templates/index.html`.
     *
     * # Examples
     *
     * Check examples of @ref WebServer::add_listener(), to see how to add the function to process the `GET /`.
     *
     * @see WebServer::add_listener()
     */
    requests::Response get(requests::Request request);
} // namespace web_server::routes::index

#endif // ROUTES_INDEX_HPP
