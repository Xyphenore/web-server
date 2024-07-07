use std::path::Path;
use std::thread::sleep;
use std::time::Duration;

use crate::requests::{Request, Response, Status};

/// Process the `GET /slow_request`.
///
/// The function sleeps 5 secondes before returning the response, to simulate a slow
/// request to process.
///
/// # Parameters
///
/// - request: [`Request`]
///
/// # Returns
///
/// [`Response`] - The response to send with [`Response::send()`], with the HTML page
/// [`templates/slow_request.html`](/templates/slow_request.html).
///
/// # Panics
///
/// If the method [`Response::add_file()`] returns an error when adding a file.
///
/// # Examples
///
/// Check examples of [`RequestHandler::add_listener()`][add_listener],
/// to see how to add the function to process the `GET /slow_request`.
///
/// <!-- References -->
///
/// [add_listener]: crate::requests::RequestHandler::add_listener()
pub fn get(request: Request) -> Response {
    let mut response = request.make_response_with_status(Status::OK);
    response
        .add_file(Path::new("templates/slow_request.html"))
        .unwrap();

    sleep(Duration::from_secs(5));
    response
}
