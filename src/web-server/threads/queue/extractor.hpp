#ifndef THREADS_QUEUE_EXTRACTOR_HPP
#define THREADS_QUEUE_EXTRACTOR_HPP

#include <memory>

#include "queue.hpp"

namespace web_server::threads::queue {
    template <typename T>
    class QueueExtractor final {
        public:
            QueueExtractor() = delete;

            explicit QueueExtractor(std::shared_ptr<Queue<T>> queue) :
                queue_{std::move(queue)} {
                if (nullptr == queue_) {
                    throw errors::InvalidQueuePointerError{};
                }
            }

            [[nodiscard]] T pop() {
                return queue_->pop();
            }

        private:
            std::shared_ptr<Queue<T>> queue_;
    };
} // namespace web_server::threads::queue

#endif // THREADS_QUEUE_EXTRACTOR_HPP
