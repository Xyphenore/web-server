#ifndef THREADS_QUEUE_INSERTER_HPP
#define THREADS_QUEUE_INSERTER_HPP

#include <memory>

#include "queue.hpp"

namespace web_server::threads::queue {
    template <typename T>
    class QueueInserter final {
        public:
            QueueInserter() = delete;

            explicit QueueInserter(std::shared_ptr<Queue<T>> queue) :
                queue_{std::move(queue)} {
                if (nullptr == queue_) {
                    throw errors::InvalidQueuePointerError{};
                }
            }

            void push(T element) {
                queue_->push(std::move(element));
            }

            void close() {
                queue_->close();
            }

        private:
            std::shared_ptr<Queue<T>> queue_;
    };
} // namespace web_server::threads::queue

#endif // THREADS_QUEUE_INSERTER_HPP
