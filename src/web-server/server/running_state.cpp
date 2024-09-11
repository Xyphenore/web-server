#include "running_state.hpp"

#include "io_context.hpp"

#include <asio/error.hpp>
#include <asio/error_code.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <csignal>
#include <iostream>

namespace web_server::server {
    RunningState::RunningState(IOContext& io_context) noexcept {
        io_context.link(
            {SIGINT, SIGTERM, SIGABRT},
            [this](const asio::error_code& error, const IOContext::Signal signal_code) {
                if (asio::error::operation_aborted != error) {
                    fmt::println(
                        std::cout,
                        FMT_STRING("Receive the signal {:d}. Start the procedure of shutdown the server."),
                        signal_code);

                    this->is_running_ = false;
                }
            });
    }
} // namespace web_server::server
