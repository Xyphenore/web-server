#include "version.hpp"

#include <web-server/helpers/errors.hpp>
#include <web-server/helpers/string.hpp>

#include <fmt/format.h>

#include <iosfwd>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

namespace web_server::requests {
    namespace errors {
        InvalidHTTPVersionError::InvalidHTTPVersionError(std::string_view version):
        std::invalid_argument{fmt::format(FMT_STRING("Invalid HTTP version: '{}'."), version)} {}
    } // namespace errors

    std::ostream& operator<<(std::ostream& os, const Version version) noexcept {
        fmt::string_view value{};

        switch (version) {
            case Version::HTTP_1 : value = "HTTP/1"; break;
            case Version::HTTP_1_1 : value = "HTTP/1.1"; break;
            case Version::HTTP_2 : value = "HTTP/2"; break;
            case Version::HTTP_3 : value = "HTTP/3"; break;
            default : helpers::panic_due_to_logic_error("The HTTP version is not supported.");
        }

        if (helpers::is_blank(value)) {
            helpers::panic_due_to_logic_error("Blank HTTP version.");
        }

        return os << helpers::to_uppercase(value);
    }

    Version to_version(const std::string_view version) {
        static const std::unordered_map<std::string, Version> ALLOWED_VERSIONS{
            {web_server::helpers::to_uppercase(fmt::to_string(Version::HTTP_1)), Version::HTTP_1},
            {web_server::helpers::to_uppercase(fmt::to_string(Version::HTTP_1_1)), Version::HTTP_1_1},
            {web_server::helpers::to_uppercase(fmt::to_string(Version::HTTP_2)), Version::HTTP_2},
            {web_server::helpers::to_uppercase(fmt::to_string(Version::HTTP_3)), Version::HTTP_3},
        };

        try {
            return ALLOWED_VERSIONS.at(helpers::to_uppercase(version));
        }
        catch (const std::out_of_range&) {
            throw errors::InvalidHTTPVersionError{version};
        }
    }
} // namespace web_server::requests
