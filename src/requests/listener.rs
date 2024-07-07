use super::{Request, Response};

pub type HTTPListener = fn(Request) -> Response;
