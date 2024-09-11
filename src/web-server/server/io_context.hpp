#ifndef SERVER_IO_CONTEXT_HPP
#define SERVER_IO_CONTEXT_HPP

#include <web-server/helpers/sockets.hpp>
#include <web-server/helpers/threads.hpp>

#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/signal_set.hpp>

#include <functional>
#include <initializer_list>
#include <list>

namespace web_server::server {
    class [[nodiscard]] IOContext final {
        public:
            enum class [[nodiscard]] BlockingMode : bool {
                On = true,
                Off = false,
            };

            IOContext();

            ~IOContext() noexcept;

            using Signal = int;
            using SignalHandler = std::function<void(const asio::error_code&, Signal)>;

            void link(Signal signal_id, SignalHandler handler);
            void link(std::initializer_list<Signal> signal_ids, SignalHandler handler);

            using Acceptor = helpers::Acceptor;
            using Listener = helpers::Listener;
            using Address = helpers::LocalEndPoint;
            using Socket = helpers::Socket;

            [[nodiscard]] Acceptor make_acceptor(const Address& address);

            [[nodiscard]] Listener listen(const Address& address);
            [[nodiscard]] Listener listen(const Address& address, BlockingMode blocking_mode);

            [[nodiscard]] Socket make_socket();

        private:
            using Executor = asio::io_context::executor_type;

            asio::io_context io_context_;

            // Memory corruption of the IOContext instance, when using an asio::executor_work_guard
            // => Solution: directly uses the executor and do the same work that the asio::executor_work_guard.
            // ------------------------------------------------------------------------------------
            // → In more details:
            // The library asio uses PImpl paradigm, so the compiler does not know type sizes and makes a bad memory
            // structuration and has the impossibility to know type interfaces.
            // The IOContext class needs to store the asio::executor_work_guard to reset it and stop the background
            // thread.
            Executor executor_{io_context_.get_executor()};

            helpers::Thread bg_task_;

            std::list<asio::signal_set> signals_;
    };
} // namespace web_server::server

namespace web_server::server {
    inline void IOContext::link(const Signal signal_id, SignalHandler handler) {
        this->link({signal_id}, std::move(handler));
    }

    inline IOContext::Acceptor IOContext::make_acceptor(const Address& address) {
        return Acceptor{io_context_, address};
    }

    inline IOContext::Socket IOContext::make_socket() { return Socket{io_context_}; }

    inline IOContext::Listener IOContext::listen(const Address& address) {
        return this->listen(address, BlockingMode::On);
    }
} // namespace web_server::server

#endif // SERVER_IO_CONTEXT_HPP
