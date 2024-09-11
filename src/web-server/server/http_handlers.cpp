#include "http_handlers.hpp"

#include <web-server/helpers/errors.hpp>
#include <web-server/routes/not_found.hpp>

#include <fmt/format.h>

#include <stdexcept>

namespace web_server::server {
    void HTTPHandlers::link(Method method, Handler handler) noexcept {
        if (const auto [_, inserted] = handlers_.try_emplace(std::move(method), std::move(handler)); not inserted) {
            helpers::panic_due_to_logic_error(
                fmt::format(FMT_STRING("The method {} has already a listener registered."), method));
        }
    }

    HTTPHandlers::Handler HTTPHandlers::linked_to(const Method& method) const noexcept {
        try {
            return handlers_.at(method);
        }
        catch (const std::out_of_range&) {
            return routes::not_found::handler;
        }
    }
} // namespace web_server::server
