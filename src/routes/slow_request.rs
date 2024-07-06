use std::path::Path;
use std::thread;
use std::time::Duration;

use crate::requests::{Request, Response, Status};

pub fn get(request: Request) -> Response {
    let mut response = request.make_response_with_status(Status::OK);
    response
        .add_file(Path::new("templates/slow_request.html"))
        .unwrap();

    thread::sleep(Duration::from_secs(5));
    response
}
