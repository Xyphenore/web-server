use std::path::Path;
use std::thread::sleep;
use std::time::Duration;

use crate::requests::{Request, Response, Status};

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
pub fn get(request: Request) -> Response {
    let mut response = Response::from((request, Status::OK));
    response
        .add_file(Path::new("templates/slow_request.html"))
        .unwrap();

    sleep(Duration::from_secs(5));
    response
}
