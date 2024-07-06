use std::path::Path;

use crate::requests::{Request, Response, Status};

pub fn get(request: Request) -> Response {
    let mut response = Response::new(request.version, Status::OK);
    response
        .add_file(Path::new("templates/index.html"))
        .unwrap();

    response
}
