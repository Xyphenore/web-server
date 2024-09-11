#include "status.hpp"

#include <web-server/helpers/errors.hpp>
#include <web-server/helpers/string.hpp>

#include <fmt/base.h>
#include <fmt/format.h>

#include <iosfwd>

namespace web_server::requests {
    std::ostream& operator<<(std::ostream& os, const Status status) noexcept {
        fmt::string_view value{};

        switch (status) {
            case Status::Ok : value = "OK"; break;
            case Status::NotFound : value = "NOT FOUND"; break;
            case Status::UnprocessableContent : value = "UNPROCESSABLE CONTENT"; break;
            default :
                helpers::panic_due_to_logic_error(
                    fmt::format(FMT_STRING("The status code {:d} is not allowed."), fmt::enums::format_as(status)));
        }

        if (helpers::is_blank(value)) {
            helpers::panic_due_to_logic_error("The status name is blank.");
        }

        return os << fmt::format(FMT_STRING("{} {}"), fmt::enums::format_as(status), helpers::to_uppercase(value));
    }
} // namespace web_server::requests
