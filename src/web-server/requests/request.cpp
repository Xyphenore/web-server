#include "request.hpp"

#include "errors.hpp"
#include "status.hpp"

// Need to load the formatter of asio::ip::address
// ReSharper disable once CppUnusedIncludeDirective
#include <web-server/helpers/compat.hpp>
#include <web-server/helpers/sockets.hpp>

#include <asio/buffer.hpp>
#include <asio/read_until.hpp>
#include <asio/write.hpp>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <regex>
#include <string>
#include <utility>

namespace {
    using web_server::requests::Request;
    using web_server::requests::errors::InvalidHTTPRequestError;
    using web_server::requests::errors::MessagePartiallySentError;
    using web_server::requests::errors::ReceiveTooBigMessageError;

    using web_server::requests::Status;

    using Method = Request::Method;
    using Verb = Method::Verb;
    using TCPStream = Request::Stream;

    const std::regex FIRST_LINE_REGEX{
        fmt::format(
            FMT_STRING("^({}) ({}) (HTTP/(?:1.1|(?:[1-3](?:.0)?)))\r\n"),
            fmt::join(
                {
                    Verb::Get,
                    Verb::Post,
                    Verb::Update,
                    Verb::Patch,
                    Verb::Delete,
                    Verb::Head,
                    Verb::Options,
                    Verb::Trace,
                    Verb::Connect,
                },
                "|"),
            Method::URI::REGEX_STRING),
        std::regex::ECMAScript | std::regex::icase | std::regex::optimize,
    };

    [[nodiscard]] std::string read(TCPStream& stream) {
        std::string content{};

        if (const auto read_size = asio::read_until(stream, asio::dynamic_buffer(content), "\r\n");
            content.max_size() <= read_size) {
            std::smatch match{};
            if (const auto matched = std::regex_search(content, match, ::FIRST_LINE_REGEX); not matched) {
                throw InvalidHTTPRequestError{content};
            }

            auto version = match.str(3);

            const auto error_response = fmt::format(FMT_STRING("{} {}\r\n\r\n"), version, Status::UnprocessableContent);

            const auto remote_address = stream.remote_endpoint().address();

            if (const auto sent_size = asio::write(stream, asio::buffer(error_response));
                error_response.size() > sent_size) {
                web_server::helpers::close(stream);
                throw MessagePartiallySentError{error_response.size() - sent_size, remote_address};
            }

            web_server::helpers::close(stream);
            throw ReceiveTooBigMessageError{remote_address};
        }

        return content;
    }
} // namespace

namespace web_server::requests {
    namespace errors {
        ReceiveTooBigMessageError::ReceiveTooBigMessageError(const Address& client_address):
        std::length_error{
            fmt::format(FMT_STRING("Error: the request received from the client ({}) is too big."), client_address),
        } {}

        InvalidHTTPRequestError::InvalidHTTPRequestError(std::string_view line) noexcept:
        std::invalid_argument{
            fmt::format(
                FMT_STRING("The HTTP request is invalid, cannot extract the first line. The given request: '{}'"),
                line),
        } {}
    } // namespace errors

    Request Request::from(Stream&& stream) {
        using StringMatchSize = std::smatch::size_type;

        static constexpr StringMatchSize VERB_INDEX{1};
        static constexpr StringMatchSize URI_INDEX{2};
        static constexpr StringMatchSize VERSION_INDEX{3};

        const auto content = read(stream);

        std::smatch match{};
        if (const auto matched = std::regex_search(content, match, ::FIRST_LINE_REGEX); not matched) {
            throw errors::InvalidHTTPRequestError{content};
        }

        return Request{
            Method{to_verb(match.str(VERB_INDEX)), to_uri(match.str(URI_INDEX))},
            to_version(match.str(VERSION_INDEX)),
            std::move(stream),
        };
    }
} // namespace web_server::requests
