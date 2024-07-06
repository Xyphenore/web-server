use std::path::Path;
use std::thread;
use std::time::Duration;

use crate::requests::{Request, Response, Status};

pub fn get(request: Request) -> Response {
    let mut response = Response::new(request.version, Status::OK);

    thread::sleep(Duration::from_secs(5));

    response
        .add_file(Path::new("templates/slow_request.html"))
        .unwrap();

    response
}
