#include "slow_request.hpp"

#include "templates_directory.hpp"

#include <web-server/requests/request.hpp>
#include <web-server/requests/response.hpp>
#include <web-server/requests/status.hpp>

#include <chrono>
#include <thread>
#include <utility>

namespace {
    constexpr std::chrono::seconds SLEEP_TIME{5};
} // namespace

namespace web_server::routes::slow_request {
    using requests::Request;
    using requests::Response;
    using requests::Status;
    using tools::templates_directory;

    Response get(Request request) {
        auto response = Response::from(std::move(request), Status::Ok);
        response.add_file(templates_directory() / "slow_request.html");

        std::this_thread::sleep_for(SLEEP_TIME);

        return response;
    }
} // namespace web_server::routes::slow_request
