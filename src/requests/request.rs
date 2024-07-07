use std::io::{BufRead, BufReader};
use std::net::TcpStream;

use super::{HTTPListener, Job, Method, Response, Status, Version};

#[derive(Debug)]
pub struct Request {
    pub method: Method,
    pub version: &'static Version,
    pub other_lines: Vec<String>,
    pub stream: TcpStream,
}

impl Request {
    pub fn from_stream(stream: TcpStream) -> Self {
        let buffer_reader = BufReader::new(&stream);
        let mut http_request: Vec<_> = buffer_reader
            .lines()
            .map(|result| result.unwrap())
            .take_while(|line| !line.is_empty())
            .collect();

        let first_line = http_request.remove(0);
        let method = Method::from_line(&first_line).unwrap();

        let mut parts = first_line.split(' ');
        // Drop the method verb
        parts.next();
        // Drop the URI
        parts.next();

        Request {
            method,
            version: Version::from(String::from_iter(parts)).unwrap(),
            other_lines: http_request,
            stream,
        }
    }

    pub fn make_response_with_status(self, status: &'static Status) -> Response {
        Response::new(self.version, status, self.stream)
    }

    pub fn make_job_with_listener(self, listener: HTTPListener) -> Job {
        Job::new(self, listener)
    }
}
