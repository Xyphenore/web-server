#ifndef ROUTES_SLOW_REQUEST_HPP
#define ROUTES_SLOW_REQUEST_HPP

#include <web-server/requests/request.hpp>
#include <web-server/requests/response.hpp>

/**
 * The module contains all functions that process requests for the URI `/slow_request`.
 *
 * # Examples
 *
 * Check examples of @ref WebServer::add_listener(), to see how to add a route to the server.
 */
namespace web_server::routes::slow_request {
    /**
     * Process the `GET /slow_request`.
     *
     * The function sleeps 5 secondes before returning the response, to simulate a slow request to process.
     *
     * @returns Response - The reponse with the HTML page @ref `templates/slow_request.html`.
     *
     * # Examples
     *
     * Check examples of @ref WebServer::add_listener(), to see how to add the function to process the
     * `GET /slow_request`.
     *
     * @see WebServer::add_listener()
     */
    requests::Response get(requests::Request request);
} // namespace web_server::routes::slow_request

#endif // ROUTES_SLOW_REQUEST_HPP
