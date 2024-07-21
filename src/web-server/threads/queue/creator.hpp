#ifndef THREADS_QUEUE_CREATOR_HPP
#define THREADS_QUEUE_CREATOR_HPP

#include <memory>
#include <utility>

#include "queue.hpp"
#include "extractor.hpp"
#include "inserter.hpp"

namespace web_server::threads::queue {
    template <typename T>
    [[nodiscard]] std::pair<QueueInserter<T>, QueueExtractor<T>> make_queue() noexcept {
        std::shared_ptr<Queue<T>> queue{
            new Queue<T>{},
            [](Queue<T>* ptr) {
                if (nullptr != ptr) {
                    try {
                        ptr->close();
                    }
                    catch (const errors::QueueClosedException&) {
                        // Do nothing, just a security to close the queue before deleting the pointer
                    }

                    delete ptr;
                }
            }};

        return std::make_pair(QueueInserter(queue), QueueExtractor(queue));
    }
} // namespace web_server::threads::queue

#endif // THREADS_QUEUE_CREATOR_HPP
