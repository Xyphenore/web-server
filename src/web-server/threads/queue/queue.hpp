#ifndef THREADS_QUEUE_HPP
#define THREADS_QUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <list>
#include <stdexcept>
#include <string>


namespace web_server::threads::queue {
    namespace errors {
        enum class TriedOperations: uint8_t {
            CLOSE,
            POP,
            PUSH,
        };

        class InvalidTriedOperationError final: public std::domain_error {
            public:
                InvalidTriedOperationError() noexcept:
                    std::domain_error{"Invalid TriedOperations value."} {}
        };

        class QueueClosedException final: public std::runtime_error {
            public:
                explicit QueueClosedException(const TriedOperations operation) noexcept :
                    std::runtime_error{
                        "Cannot do the operation '"
                        + (format(operation) += "', the queue is already closed.")
                    } {}

            private:
                [[nodiscard]] static std::string format(const TriedOperations operation) {
                    std::string value{};

                    switch (operation) {
                        case TriedOperations::POP: value = "POP";
                            break;
                        case TriedOperations::PUSH: value = "PUSH";
                            break;
                        case TriedOperations::CLOSE: value = "CLOSE";
                            break;
                        default: break;
                    }

                    if (value.empty()) {
                        throw errors::InvalidTriedOperationError{};
                    }

                    return value;
                }
        };

        class InvalidQueuePointerError final: public std::invalid_argument {
            public:
                InvalidQueuePointerError() noexcept :
                    std::invalid_argument{"Queue pointer is null."} {}
        };
    } // namespace errors

    template <typename T>
    class Queue final {
        public:
            void close() {
                {
                    const std::lock_guard lk{mutex_};

                    if (closed_) {
                        throw errors::QueueClosedException{errors::TriedOperations::CLOSE};
                    }
                    closed_ = true;
                }

                elements_cv_.notify_all();
            }

            void push(T element) {
                {
                    const std::lock_guard lk{mutex_};

                    if (closed_) {
                        throw errors::QueueClosedException{errors::TriedOperations::PUSH};
                    }

                    elements_.push_back(std::move(element));
                }

                elements_cv_.notify_one();
            }

            [[nodiscard]] T pop() {
                {
                    const std::lock_guard lk{mutex_};

                    if (!elements_.empty()) {
                        T element{std::move(elements_.front())};
                        elements_.pop_front();

                        return element;
                    }

                    if (closed_) {
                        throw errors::QueueClosedException{errors::TriedOperations::POP};
                    }
                }

                std::unique_lock lk{mutex_};
                elements_cv_.wait(lk, [this] { return this->check_has_an_element(); });

                T element{std::move(elements_.front())};
                elements_.pop_front();

                return element;
            }

            [[nodiscard]] bool is_closed() const noexcept {
                const std::lock_guard lk{mutex_};
                return closed_;
            }

        private:
            /**
             * Check if an element is available, and returns True if it is the case.
             *
             * If no element is available, throws [`QueueClosedException`] if the queue is closed.
             *
             * @warning The mutex must be locked before calling this function.
             * @warning This function must be used with [`std::condition_variable::wait`].
             *
             * @return bool - True if an element is available.
             */
            [[nodiscard]] bool check_has_an_element() {
                if (!elements_.empty()) {
                    return true;
                }

                if (closed_) {
                    throw errors::QueueClosedException{errors::TriedOperations::POP};
                }

                return false;
            }

            std::list<T> elements_{};
            bool closed_ = false;

            std::mutex mutex_{};
            std::condition_variable elements_cv_{};
    };
} // namespace web_server::threads::queue

#endif // THREADS_QUEUE_HPP
