use std::fmt::{Display, Formatter};
use std::fs;
use std::io::Write;
use std::net::TcpStream;
use std::path::Path;

use super::{Status, Version};

#[derive(Debug)]
pub struct Response {
    #[doc(hidden)]
    version: &'static Version,
    #[doc(hidden)]
    status: &'static Status,
    #[doc(hidden)]
    contents: String,
    #[doc(hidden)]
    stream: TcpStream,
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

impl Response {
    pub fn new(version: &'static Version, status: &'static Status, stream: TcpStream) -> Self {
        Self {
            version,
            contents: String::new(),
            status,
            stream,
        }
    }

    pub fn add_file(&mut self, path: impl AsRef<Path>) -> Result<&mut Self, std::io::Error> {
        fs::read_to_string(path.as_ref()).map(|contents| {
            self.contents += &contents.to_owned();
            self
        })
    }

    pub fn send(&mut self) {
        self.stream.write_all(self.to_string().as_bytes()).unwrap()
    }
}
