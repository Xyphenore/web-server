#include "io_context.hpp"

#include <web-server/helpers/errors.hpp>

#include <initializer_list>
#include <iostream>
#include <tuple>

namespace web_server::server {
    // Dev note: constructor and destructor must be cpp file instead of hpp file
    // even if their size is small, because asio::io_context::executor_type is known only
    // at execution time and make a compilation error.
    // For more explanations, check the note in the private section of the IOContext class in the hpp file.

    IOContext::IOContext():
    bg_task_{[this] {
        // Add a ghost work to the executor in the thread function, because the thread starts directly after it is
        // built.
        this->executor_.on_work_started();
        std::ignore = this->io_context_.run();
    }} {}

    IOContext::~IOContext() noexcept { executor_.on_work_finished(); }

    void IOContext::link(const std::initializer_list<Signal> signal_ids, SignalHandler handler) {
        if (0 == signal_ids.size()) {
            helpers::panic_due_to_a_logic_error("The signal list is empty.");
        }

        auto& signals = signals_.emplace_back(io_context_);
        for (const auto signal_id : signal_ids) {
            signals.add(signal_id);
        }

        signals.async_wait(handler);
    }

    IOContext::Listener IOContext::listen(const Address& address, const BlockingMode blocking_mode) {
        auto listener = this->make_acceptor(address);
        listener.non_blocking(BlockingMode::Off == blocking_mode);

        std::cout << "Server started and waiting for incoming connections on '" << listener.local_endpoint() << "'.\n";

        return listener;
    }
} // namespace web_server::server
