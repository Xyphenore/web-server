#include "not_found.hpp"

#include "templates_directory.hpp"

#include <web-server/requests/request.hpp>
#include <web-server/requests/response.hpp>
#include <web-server/requests/status.hpp>

#include <utility>

namespace web_server::routes::not_found {
    using requests::Request;
    using requests::Response;
    using requests::Status;
    using tools::TEMPLATES_DIRECTORY;

    Response handler(Request request) {
        auto response = Response::from(std::move(request), Status::NotFound);
        response.add_file(TEMPLATES_DIRECTORY / "not_found.html");

        return response;
    }

} // namespace web_server::routes::not_found
