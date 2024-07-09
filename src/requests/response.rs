use std::fmt::{Display, Formatter};
use std::fs;
use std::io::Write;
use std::net::TcpStream;
use std::path::Path;

use super::{Request, Status, Version};

/// HTTP response.
///
/// # How to use it?
///
/// ```rust
/// use std::net::TcpListener;
///
/// use crate::requests::{Request, Status};
///
/// let listener = TcpListener::bind("127.0.0.1:8000").unwrap();
///
/// for stream in listener.incoming() {
///     let request = Request::from_stream(stream.unwrap());
///
///     let response = request.make_response_with_status(Status::OK);
///     response.send();
/// }
/// ```
#[derive(Debug)]
pub struct Response {
    #[doc(hidden)]
    version: Version,
    #[doc(hidden)]
    status: Status,
    #[doc(hidden)]
    contents: String,
    #[doc(hidden)]
    stream: TcpStream,
}

impl Response {
    /// Add the content of the file to the [`Response`].
    ///
    /// # Returns
    ///
    /// Returns the instance [`Response`], or [`std::io::Error`] if an error happened
    /// during the read of the file.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use std::net::TcpListener;
    /// use std::path::Path;
    ///
    /// use crate::requests::{Request, Status};
    ///
    /// let listener = TcpListener::bind("127.0.0.1:8000").unwrap();
    ///
    /// for stream in listener.incoming() {
    ///     let request = Request::from_stream(stream.unwrap());
    ///
    ///     let response = request.make_response_with_status(Status::OK);
    ///     response.add_file(Path::new("file_to_be_loaded"));
    ///     response.send();
    /// }
    /// ```
    pub fn add_file(&mut self, path: impl AsRef<Path>) -> Result<&mut Response, std::io::Error> {
        fs::read_to_string(path.as_ref()).map(|contents| {
            self.contents += &contents.to_owned();
            self
        })
    }

    /// Send the response to the stream [`TcpStream`].
    ///
    /// # Examples
    ///
    /// ```rust
    /// use std::net::TcpListener;
    ///
    /// use crate::requests::{Request, Status};
    ///
    /// let listener = TcpListener::bind("127.0.0.1:8000").unwrap();
    ///
    /// for stream in listener.incoming() {
    ///     let request = Request::from_stream(stream.unwrap());
    ///
    ///     let response = request.make_response_with_status(Status::OK);
    ///     response.send();
    /// }
    /// ```
    ///
    /// # Panics
    ///
    /// - If the [`TcpStream::write_all`] panics.
    pub fn send(&mut self) {
        self.stream.write_all(self.to_string().as_bytes()).unwrap()
    }
}

impl Display for Response {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "{} {}\r\nContent-Length: {}\r\n\r\n{}",
            &self.version,
            &self.status,
            self.contents.len(),
            &self.contents,
        )
    }
}

impl From<(Request, Status)> for Response {
    /// Create a [`Response`] with a [`Status`] from the [`Request`] and consume it.
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`Response`].
    fn from(value: (Request, Status)) -> Response {
        let request = value.0;
        let (_, version, stream) = request.take_content();

        Self {
            version,
            contents: String::new(),
            status: value.1,
            stream,
        }
    }
}
