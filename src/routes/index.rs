use std::fs;

use crate::requests::Request;

pub fn get(request: Request) -> String {
    let version = request.version;

    let code = "200 OK";

    let response_header = format!("{version} {code}");

    let response_body = fs::read_to_string("templates/index.html").unwrap();
    let length = response_body.len();

    format!("{response_header}\r\nContent-Length: {length}\r\n\r\n{response_body}")
}
