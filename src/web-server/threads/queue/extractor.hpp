#ifndef THREADS_QUEUE_EXTRACTOR_HPP
#define THREADS_QUEUE_EXTRACTOR_HPP

#include "queue.hpp"

#include <web-server/helpers/errors.hpp>

#include <memory>
#include <utility>

namespace web_server::threads::queue {
    template <typename T>
    class [[nodiscard]] QueueExtractor final {
        public:
            using Queue = Queue<T>;
            using QueuePointer = std::shared_ptr<Queue>;

            QueueExtractor() noexcept = delete;

            explicit QueueExtractor(QueuePointer queue) noexcept;

            [[nodiscard]] T pop();

        private:
            friend void swap(QueueExtractor& lhs, QueueExtractor& rhs) noexcept { std::swap(lhs.queue_, rhs.queue_); }

            [[nodiscard]] friend bool operator==(const QueueExtractor& lhs, const QueueExtractor& rhs) noexcept {
                return lhs.queue_ == rhs.queue_;
            }

            [[nodiscard]] friend bool operator!=(const QueueExtractor& lhs, const QueueExtractor& rhs) noexcept {
                return not(lhs == rhs);
            }

            QueuePointer queue_;
    };
} // namespace web_server::threads::queue

namespace web_server::threads::queue {
    template <typename T>
    QueueExtractor<T>::QueueExtractor(QueuePointer queue) noexcept: queue_{std::move(queue)} {
        if (nullptr == queue_) {
            helpers::panic_due_to_logic_error("Queue pointer is null.");
        }
    }

    template <typename T>
    T QueueExtractor<T>::pop() {
        if (nullptr == queue_) {
            helpers::panic_due_to_logic_error("Queue pointer is null. You use a moved extractor.");
        }

        return queue_->pop();
    }
} // namespace web_server::threads::queue

#endif // THREADS_QUEUE_EXTRACTOR_HPP
