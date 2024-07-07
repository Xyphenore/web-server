use std::io::{BufRead, BufReader};
use std::net::TcpStream;

use super::{HTTPListener, Job, Method, Response, Status, Version};

/// HTTP request.
///
/// # How to create it?
///
/// ```rust
/// use std::net::TcpListener;
///
/// use crate::requests::Request;
///
/// let listener = TcpListener::bind("127.0.0.1:8000").unwrap();
///
/// for stream in listener.incoming() {
///     let request = Request::from_stream(stream.unwrap());
///
///     // Process the request after.
/// }
/// ```
#[derive(Debug)]
pub struct Request {
    pub method: Method,
    #[doc(hidden)]
    version: &'static Version,
    /// Other lines read from the stream.
    pub _other_lines: Vec<String>,
    #[doc(hidden)]
    stream: TcpStream,
}

impl Request {
    /// Create a [`Request`] from a [`TcpStream`].
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`Request`].
    ///
    /// # Panics
    ///
    /// - If the read of `stream` fails.
    /// - If the read of the method from the `stream`.
    /// - If the read of the version from the `stream`.
    pub fn from_stream(stream: TcpStream) -> Request {
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
            _other_lines: http_request,
            stream,
        }
    }

    /// Create a [`Response`] with a [`Status`] from the [`Request`] and consume it.
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`Response`].
    pub fn make_response_with_status(self, status: &'static Status) -> Response {
        Response::new(self.version, status, self.stream)
    }

    /// Create a [`Job`] with a [`HTTPListener`] from the [`Request`] and consume it.
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`Job`].
    pub fn make_job_with_listener(self, listener: HTTPListener) -> Job {
        Job::new(self, listener)
    }
}
