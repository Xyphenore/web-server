use std::path::Path;

use crate::requests::{Request, Response, Status};

/// Process the `GET /`.
///
/// # Parameters
///
/// - request: [`Request`]
///
/// # Returns
///
/// [`Response`] - The response to send with [`Response::send()`], with the HTML page
/// [`templates/index.html`](/templates/index.html).
///
/// # Panics
///
/// If the method [`Response::add_file()`] returns an error when adding a file.
///
/// # Examples
///
/// Check examples of [`RequestHandler::add_listener()`][add_listener],
/// to see how to add the function to process the `GET /`.
///
/// <!-- References -->
///
/// [add_listener]: crate::requests::RequestHandler::add_listener()
pub fn get(request: Request) -> Response {
    let mut response = request.make_response_with_status(Status::OK);
    response
        .add_file(Path::new("templates/index.html"))
        .unwrap();

    response
}
