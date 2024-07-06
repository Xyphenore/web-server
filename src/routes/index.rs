use std::path::Path;

use crate::requests::{Request, Response, Status};

pub fn get(request: Request) -> Response {
    let mut response = request.make_response_with_status(Status::OK);
    response
        .add_file(Path::new("templates/index.html"))
        .unwrap();

    response
}
