#ifndef REQUESTS_JOB_HPP
#define REQUESTS_JOB_HPP

#include <utility>

#include "response.hpp"
#include "request.hpp"

namespace web_server::requests {
    /// Type for functions that can process a [`Request`] and returns a [`Response`].
    ///
    /// # Examples
    ///
    /// Check examples of [`WebServer`](crate::server::WebServer).
    using HTTPListener = std::function<Response (Request)>;

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
    class Job final {
        public:
            Job(Request request, HTTPListener listener) noexcept:
                request_{std::move(request)}, listener_{std::move(listener)} {}

            /// Call the listener with the request.
            ///
            /// # Returns
            ///
            /// Returns the [`Response`] created by the [`HTTPListener`].
            ///
            /// # Panics
            ///
            /// - If the [`HTTPListener`] panics.
            [[nodiscard]] Response execute() {
                return listener_(std::move(request_));
            }

        private:
            Request request_;
            HTTPListener listener_;
    };
} // namespace web_server::requests

#endif // REQUESTS_JOB_HPP
