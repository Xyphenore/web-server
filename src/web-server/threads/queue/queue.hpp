#ifndef THREADS_QUEUE_QUEUE_HPP
#define THREADS_QUEUE_QUEUE_HPP

#include <web-server/helpers/errors.hpp>

#include <condition_variable>
#include <list>
#include <mutex>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace web_server::threads::queue {
    namespace errors {
        class [[nodiscard]] QueueClosedException final: public std::runtime_error {
            public:
                QueueClosedException() noexcept;
        };
    } // namespace errors

    template <typename T>
    class [[nodiscard]] Queue final {
            static_assert(std::is_destructible_v<T>, "T must be Erasable.");
            static_assert(std::is_move_constructible_v<T> and std::is_move_assignable_v<T>, "T must be MoveInsertable");

        public:
            Queue() noexcept = default;

            void close() noexcept;

            void push(T element) noexcept;

            [[nodiscard]] T pop();

        private:
            mutable std::mutex mutex_;
            mutable std::condition_variable elements_cv_;

            std::list<T> elements_{};
            bool is_closed_{false};
    };
} // namespace web_server::threads::queue

namespace web_server::threads::queue {
    namespace errors {
        inline QueueClosedException::QueueClosedException() noexcept:
        std::runtime_error{"Cannot do the operation 'POP', the queue is already closed."} {}
    } // namespace errors

    template <typename T>
    void Queue<T>::close() noexcept {
        {
            const std::lock_guard lk{mutex_};

            if (is_closed_) {
                helpers::panic_due_to_logic_error("Cannot do the operation 'CLOSE', the queue is already closed.");
            }
            is_closed_ = true;
        }

        elements_cv_.notify_all();
    }

    template <typename T>
    void Queue<T>::push(T element) noexcept {
        {
            const std::lock_guard lk{mutex_};

            if (is_closed_) {
                helpers::panic_due_to_logic_error("Cannot do the operation 'PUSH', the queue is already closed.");
            }

            elements_.push_back(std::move(element));
        }

        elements_cv_.notify_one();
    }

    template <typename T>
    T Queue<T>::pop() {
        std::unique_lock lk{mutex_};
        elements_cv_.wait(lk, [this] { return (this->is_closed_) or (not this->elements_.empty()); });

        if (not elements_.empty()) {
            T element{std::move(elements_.front())};
            elements_.pop_front();

            return element;
        }

        if (is_closed_) {
            throw errors::QueueClosedException{};
        }

        helpers::panic_due_to_logic_error("Queue is empty, it is not possible to pop an element.");
    }
} // namespace web_server::threads::queue

#endif // THREADS_QUEUE_QUEUE_HPP
