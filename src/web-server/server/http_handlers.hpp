#ifndef SERVER_HTTP_HANDLERS_HPP
#define SERVER_HTTP_HANDLERS_HPP

#include <web-server/requests/job.hpp>

#include <unordered_map>
#include <utility>

namespace web_server::server {
    class [[nodiscard]] HTTPHandlers final {
        public:
            using Job = requests::Job;
            using Handler = Job::Handler;
            using Method = Job::Request::Method;

            HTTPHandlers() noexcept = default;

            void link(Method method, Handler handler) noexcept;

            Handler linked_to(const Method& method) const noexcept;

        private:
            friend void swap(HTTPHandlers& lhs, HTTPHandlers& rhs) noexcept { std::swap(lhs.handlers_, rhs.handlers_); }

            std::unordered_map<Method, Handler> handlers_;
    };
} // namespace web_server::server

#endif // SERVER_HTTP_HANDLERS_HPP
