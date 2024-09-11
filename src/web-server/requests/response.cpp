#include "response.hpp"

#include "errors.hpp"

#include <web-server/helpers/errors.hpp>
#include <web-server/helpers/sockets.hpp>

#include <asio/buffer.hpp>
#include <asio/write.hpp>

#include <fmt/format.h>
#include <fmt/std.h>

#include <cerrno>
#include <fstream>
#include <string>

namespace web_server::requests {
    void Response::add_file(const Path& filename) {
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

            (contents_ += read_content) += '\n';
        }
    }

    void Response::send() {
        const auto message = fmt::format(
            FMT_STRING("{} {}\r\nContent-Length: {:d}\r\n\r\n{}"),
            version_,
            status_,
            contents_.size(),
            contents_);

        if (const auto sent_size = asio::write(stream_, asio::buffer(message)); message.size() != sent_size) {
            throw errors::MessagePartiallySentError{
                message.size() - sent_size,
                stream_.remote_endpoint().address(),
            };
        }

        helpers::close(stream_);
    }
} // namespace web_server::requests
