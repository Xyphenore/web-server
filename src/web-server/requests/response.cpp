#include "response.hpp"

#include "errors.hpp"

#include <web-server/helpers/errors.hpp>
#include <web-server/helpers/sockets.hpp>

#include <asio/buffer.hpp>
#include <asio/write.hpp>

#include <fmt/format.h>
#include <fmt/std.h>

#ifdef USE_RANGES
    #include <range/v3/action/push_back.hpp>
    #include <range/v3/view/for_each.hpp>
    #include <range/v3/view/getlines.hpp>
#endif // USE_RANGES

#include <cerrno>
#include <fstream>
#include <string>

#ifndef USE_RANGES
    #include <tuple>
#endif // USE_RANGES

namespace web_server::requests {
#ifndef USE_RANGES
    void Response::add_file(const Path& filename) {
        if (this->has_content()) {
            helpers::panic(fmt::format(
                FMT_STRING("Cannot add the file: '{}'. The reason: 'The response already has a content'."),
                filename));
        }

        std::ifstream file{filename};
        if (not file.good()) {
            helpers::panic(fmt::format(
                FMT_STRING("Cannot open the file: '{}'. The reason: '{}'."),
                filename,
                helpers::string_error(errno)));
        }

        std::string read_content{};

        while (not file.eof()) {
            file.clear();

            std::ignore = std::getline(file, read_content);

            if (file.bad()) {
                helpers::panic(fmt::format(
                    FMT_STRING("Cannot read a line from the file: '{}'. The reason: '{}'."),
                    filename,
                    helpers::string_error(errno)));
            }

            if (file.fail() and (not file.eof())) {
                if (read_content.size() != read_content.max_size()) {
                    helpers::panic(fmt::format(
                        FMT_STRING("Cannot read a line from the file: '{}'. The reason: '{}'."),
                        filename,
                        helpers::string_error(errno)));
                }

                helpers::panic(fmt::format(
                    FMT_STRING("The file '{}' cannot be stored in a std::string, the file is too big."),
                    filename));
            }

            (content_ += read_content) += '\n';
        }
    }
#else // USE_RANGES
    void Response::add_file(const Path& filename) {
        static constexpr auto END_LINE{'\n'};
        static const auto add_end_line = [](std::string line) {
            line += END_LINE;
            return line;
        };

        if (this->has_content()) {
            helpers::panic(fmt::format(
                FMT_STRING("Cannot add the file: '{}'. The reason: 'The response already has a content'."),
                filename));
        }

        content_.clear();

        std::ifstream file{filename};
        if (not file.good()) {
            helpers::panic(fmt::format(
                FMT_STRING("Cannot open the file: '{}'. The reason: '{}'."),
                filename,
                helpers::string_error(errno)));
        }

        content_ |= ranges::push_back(ranges::getlines_view{file, END_LINE} | ranges::views::for_each(add_end_line));

        if (file.bad()) {
            helpers::panic(fmt::format(
                FMT_STRING("Cannot read a line from the file: '{}'. The reason: '{}'."),
                filename,
                helpers::string_error(errno)));
        }

        if (file.fail() and (not file.eof())) {
            helpers::panic(fmt::format(
                FMT_STRING("The file '{}' cannot be stored in a std::string, the file is too big."),
                filename));
        }
    }
#endif // USE_RANGES

    void Response::send() {
        const auto message = fmt::format(
            FMT_STRING("{} {}\r\nContent-Length: {:d}\r\n\r\n{}"),
            version_,
            status_,
            content_.size(),
            content_);

        if (const auto sent_size = asio::write(stream_, asio::buffer(message)); message.size() != sent_size) {
            throw errors::MessagePartiallySentError{
                message.size() - sent_size,
                stream_.remote_endpoint().address(),
            };
        }

        helpers::close(stream_);
    }
} // namespace web_server::requests
