use super::{Method, Version};

#[derive(Debug, Clone, Hash)]
pub struct Request {
    pub method: Method,
    pub version: &'static Version,
    pub other_lines: Vec<String>,
}
