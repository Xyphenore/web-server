use std::fmt::{Display, Formatter};
use std::fs;
use std::ops::Deref;
use std::path::Path;

use super::{Status, Version};

#[derive(Debug, Clone, PartialEq, PartialOrd, Hash)]
pub struct Response {
    version: &'static Version,
    status: &'static Status,
    contents: String,
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
    pub fn new(version: &'static Version, status: &'static Status) -> Response {
        Response {
            version,
            contents: String::new(),
            status,
        }
    }

    pub fn add_file(&mut self, path: impl AsRef<Path>) -> Result<&Self, std::io::Error> {
        fs::read_to_string(path.as_ref()).and_then(|contents| {
            self.contents += &contents;
            Ok(self.deref())
        })
    }
}
