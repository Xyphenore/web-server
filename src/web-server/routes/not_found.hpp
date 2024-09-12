#ifndef ROUTES_NOT_FOUND_HPP
#define ROUTES_NOT_FOUND_HPP

#include <web-server/requests/request.hpp>
#include <web-server/requests/response.hpp>

namespace web_server::routes::not_found {
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
    requests::Response handler(requests::Request request);
} // namespace web_server::routes::not_found

#endif // ROUTES_NOT_FOUND_HPP
