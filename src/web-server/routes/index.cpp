#include "index.hpp"

#include "templates_directory.hpp"

#include <web-server/requests/request.hpp>
#include <web-server/requests/response.hpp>
#include <web-server/requests/status.hpp>

#include <utility>

namespace web_server::routes::index {
    using requests::Request;
    using requests::Response;
    using requests::Status;
    using tools::templates_directory;

    Response get(Request request) {
        auto response = Response::from(std::move(request), Status::Ok);
        response.add_file(templates_directory() / "index.html");

        return response;
    }
} // namespace web_server::routes::index
