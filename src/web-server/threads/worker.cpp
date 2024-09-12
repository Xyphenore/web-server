#include "worker.hpp"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <exception>
#include <iostream>

namespace web_server::threads {
    Worker::Worker(const ID id, QueueExtractor queue) noexcept:
    handle_{
        [id](QueueExtractor queue_) noexcept {
            while (true) {
                try {
                    queue_.pop().execute().send();
                }
                catch (const queue::errors::QueueClosedException&) {
                    fmt::println(std::cout, FMT_STRING("Worker {:d} disconnected: shutting down."), id);
                    break;
                }
                catch (const std::exception& error) {
                    fmt::println(
                        std::cerr,
                        FMT_STRING("Worker {:d} disconnected due to an error: {}."),
                        id,
                        error.what());
                    break;
                }
            }
        },
        std::move(queue),
    } {}
} // namespace web_server::threads
