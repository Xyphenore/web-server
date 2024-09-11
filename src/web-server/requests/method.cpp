#include "method.hpp"

#include <web-server/helpers/errors.hpp>
#include <web-server/helpers/string.hpp>

#include <fmt/format.h>

#include <iosfwd>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace {
    using Method = web_server::requests::Method;
    using Verb = Method::Verb;

    const std::regex URI_REGEX{
        Method::URI::REGEX_STRING.cbegin(),
        Method::URI::REGEX_STRING.cend(),
        std::regex::ECMAScript | std::regex::optimize,
    };

    const std::unordered_map<std::string, Verb> ALLOWED_VERBS{
        {web_server::helpers::to_uppercase(fmt::to_string(Verb::Get)), Verb::Get},
        {web_server::helpers::to_uppercase(fmt::to_string(Verb::Post)), Verb::Post},
        {web_server::helpers::to_uppercase(fmt::to_string(Verb::Update)), Verb::Update},
        {web_server::helpers::to_uppercase(fmt::to_string(Verb::Patch)), Verb::Patch},
        {web_server::helpers::to_uppercase(fmt::to_string(Verb::Head)), Verb::Head},
        {web_server::helpers::to_uppercase(fmt::to_string(Verb::Options)), Verb::Options},
        {web_server::helpers::to_uppercase(fmt::to_string(Verb::Trace)), Verb::Trace},
        {web_server::helpers::to_uppercase(fmt::to_string(Verb::Delete)), Verb::Delete},
        {web_server::helpers::to_uppercase(fmt::to_string(Verb::Connect)), Verb::Connect},
    };
} // namespace

namespace web_server::requests {
    namespace errors {
        std::ostream& operator<<(std::ostream& os, const ElementTypes type) noexcept {
            std::string_view value{};

            switch (type) {
                case ElementTypes::Verb : value = "verb"; break;
                case ElementTypes::URI : value = "URI"; break;
                default : helpers::panic_due_to_a_logic_error("Invalid value of the enum ElementTypes.");
            }

            if (helpers::is_blank(value)) {
                helpers::panic_due_to_a_logic_error("The element type value is blank.");
            }

            return os << value;
        }
    } // namespace errors

    Method::Method(Verb verb, URI uri) noexcept: value_{fmt::format(FMT_STRING("{} {}"), verb, std::move(uri))} {}

    Method::Verb to_verb(const std::string_view verb) {
        try {
            return ALLOWED_VERBS.at(helpers::to_uppercase(verb));
        }
        catch (const std::out_of_range&) {
            throw errors::InvalidVerbError{verb};
        }
    }

    Method::URI::URI(const std::string_view value): value_{value} {
        if (not std::regex_match(value_, ::URI_REGEX)) {
            throw errors::InvalidURIError{value};
        }
    }

    std::ostream& operator<<(std::ostream& os, const Verb verb) noexcept {
        fmt::string_view value{};

        switch (verb) {
            case Verb::Get : value = "GET"; break;
            case Verb::Post : value = "POST"; break;
            case Verb::Update : value = "UPDATE"; break;
            case Verb::Delete : value = "DELETE"; break;
            case Verb::Patch : value = "PATCH"; break;
            case Verb::Options : value = "OPTIONS"; break;
            case Verb::Head : value = "HEAD"; break;
            case Verb::Connect : value = "CONNECT"; break;
            case Verb::Trace : value = "TRACE"; break;
            default : helpers::panic_due_to_a_logic_error("Value of Method::Verb without linked case.");
        }

        if (helpers::is_blank(value)) {
            helpers::panic_due_to_a_logic_error("Value of Method::Verb is blank.");
        }

        return os << helpers::to_uppercase(value);
    }
} // namespace web_server::requests
