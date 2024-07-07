use crate::requests::{HTTPListener, Request, Response};

#[derive(Debug)]
pub struct Job {
    #[doc(hidden)]
    request: Request,
    #[doc(hidden)]
    listener: HTTPListener,
}

impl Job {
    pub fn new(request: Request, listener: HTTPListener) -> Self {
        Self { request, listener }
    }

    pub fn execute(self) -> Response {
        let listener = self.listener;
        listener(self.request)
    }
}
