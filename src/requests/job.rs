use crate::requests::{HTTPListener, Request};

#[derive(Debug)]
pub struct Job {
    pub request: Request,
    pub listener: HTTPListener,
}
