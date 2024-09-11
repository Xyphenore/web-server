#ifndef REQUESTS_JOB_HPP
#define REQUESTS_JOB_HPP

#include "request.hpp"
#include "response.hpp"

#include <functional>
#include <utility>

namespace web_server::requests {
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
    class [[nodiscard]] Job final {
        public:
            using Request = Request;
            using Response = Response;

            /// Type for functions that can process a [`Request`] and returns a [`Response`].
            ///
            /// # Examples
            ///
            /// Check examples of [`WebServer`](crate::server::WebServer).
            using Handler = std::function<Response(Request)>;

            Job() noexcept = delete;

            explicit Job(Request&& request, Handler handler) noexcept;

            /// Call the listener with the request.
            ///
            /// # Returns
            ///
            /// Returns the [`Response`] created by the [`HTTPListener`].
            ///
            /// # Panics
            ///
            /// - If the [`HTTPListener`] panics.
            Response execute();

            Response operator()();

        private:
            friend void swap(Job& lhs, Job& rhs) noexcept {
                std::swap(lhs.request_, rhs.request_);
                std::swap(lhs.handler_, rhs.handler_);
            }

            Request request_;
            Handler handler_;
    };
} // namespace web_server::requests

namespace web_server::requests {
    inline Job::Job(Request&& request, Handler handler) noexcept:
    request_{std::move(request)}, handler_{std::move(handler)} {}

    inline Job::Response Job::execute() { return handler_(std::move(request_)); }

    inline Job::Response Job::operator()() { return this->execute(); }
} // namespace web_server::requests

#endif // REQUESTS_JOB_HPP
