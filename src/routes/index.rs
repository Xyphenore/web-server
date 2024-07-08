use std::path::Path;

use crate::requests::{Request, Response, Status};

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
pub fn get(request: Request) -> Response {
    let mut response = Response::from((request, Status::OK));
    response
        .add_file(Path::new("templates/index.html"))
        .unwrap();

    response
}
