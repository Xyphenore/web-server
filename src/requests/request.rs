use std::io::{BufRead, BufReader};
use std::net::TcpStream;

use super::{Method, Version};

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
///     let request = Request::from(stream.unwrap());
///
///     // Process the request after.
/// }
/// ```
#[derive(Debug)]
pub struct Request {
    #[doc(hidden)]
    method: Method,
    #[doc(hidden)]
    version: Version,
    #[doc(hidden)]
    stream: TcpStream,
}

impl Request {
    pub fn method(&self) -> &Method {
        &self.method
    }

    pub fn take_content(self) -> (Method, Version, TcpStream) {
        (self.method, self.version, self.stream)
    }
}

impl From<TcpStream> for Request {
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
    fn from(value: TcpStream) -> Request {
        let buffer_reader = BufReader::new(&value);
        let mut http_request: Vec<_> = buffer_reader
            .lines()
            .map(|result| result.unwrap())
            .take_while(|line| !line.is_empty())
            .collect();

        let first_line = http_request.remove(0);
        let method = Method::try_from(&first_line).unwrap();

        let mut parts = first_line.split(' ');
        // Drop the method verb
        parts.next();
        // Drop the URI
        parts.next();

        Request {
            method,
            version: Version::try_from(String::from_iter(parts)).unwrap(),
            stream: value,
        }
    }
}
