use crate::requests::{HTTPListener, Request, Response};

/// A transport structure for a [`Request`] and an [`HTTPListener`].
///
/// # How to create it?
///
/// ```rust
/// use crate::requests::{Request, Response, Status};
///
/// fn process(request: Request) -> Response {
///     request.make_response_with_status(Status::OK)
/// }
///
/// let request = Request::new();
///
/// let job = Job::new(request, process);
/// // Now, the job can be executed by the WorkerPool.
/// ```
///
/// # How to execute it?
///
/// ```rust
/// use crate::requests::{Request, Response, Status};
///
/// fn process(request: Request) -> Response {
///     request.make_response_with_status(Status::OK)
/// }
///
/// let request = Request::new();
///
/// let job = Job::new(request, process);
/// // Now, the job can be executed by the WorkerPool.
/// // Or, like this
/// let response = job.execute();
///
/// // Now, you need to send the response.
/// ```
#[derive(Debug)]
pub struct Job {
    #[doc(hidden)]
    request: Request,
    #[doc(hidden)]
    listener: HTTPListener,
}

impl Job {
    /// Create a new [`Job`] with the [`Request`] and the [`HTTPListener`].
    ///
    /// # Parameters
    ///
    /// - `request`: The request to process in the listener.
    /// - `listener`: The function that will process the request.
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`Job`].
    pub fn new(request: Request, listener: HTTPListener) -> Job {
        Self { request, listener }
    }

    /// Call the listener with the request.
    ///
    /// # Returns
    ///
    /// Returns the [`Response`] created by the [`HTTPListener`].
    ///
    /// # Panics
    ///
    /// - If the [`HTTPListener`] panics.
    pub fn execute(self) -> Response {
        let listener = self.listener;
        listener(self.request)
    }
}
