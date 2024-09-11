#ifndef THREADS_QUEUE_INSERTER_HPP
#define THREADS_QUEUE_INSERTER_HPP

#include "extractor.hpp"

#include <web-server/helpers/errors.hpp>

#include <memory>
#include <utility>

namespace web_server::threads::queue {
    template <typename T>
    class [[nodiscard]] QueueInserter final {
        public:
            using QueueExtractor = QueueExtractor<T>;
            using Queue = typename QueueExtractor::Queue;

            QueueInserter() noexcept = default;

            QueueInserter(const QueueInserter&) noexcept = delete;
            QueueInserter(QueueInserter&&) noexcept = default;

            QueueInserter& operator=(const QueueInserter&) noexcept = delete;
            QueueInserter& operator=(QueueInserter&&) noexcept = default;

            ~QueueInserter() noexcept;

            void push(T element) noexcept;

            QueueExtractor make_extractor() noexcept;

        private:
            friend void swap(QueueInserter& lhs, QueueInserter& rhs) noexcept { std::swap(lhs.queue_, rhs.queue_); }

            [[nodiscard]] friend bool operator==(const QueueInserter& lhs, const QueueInserter& rhs) noexcept {
                return lhs.queue_ == rhs.queue_;
            }

            [[nodiscard]] friend bool operator!=(const QueueInserter& lhs, const QueueInserter& rhs) noexcept {
                return not(lhs == rhs);
            }

            typename QueueExtractor::QueuePointer queue_{std::make_shared<Queue>()};
    };
} // namespace web_server::threads::queue

namespace web_server::threads::queue {
    template <typename T>
    void QueueInserter<T>::push(T element) noexcept {
        if (nullptr == queue_) {
            helpers::panic_due_to_logic_error("Queue pointer is null. You use a moved inserter.");
        }

        queue_->push(std::move(element));
    }

    template <typename T>
    typename QueueInserter<T>::QueueExtractor QueueInserter<T>::make_extractor() noexcept {
        if (nullptr == queue_) {
            helpers::panic_due_to_logic_error("Queue pointer is null. You use a moved inserter.");
        }

        return QueueExtractor{queue_};
    }

    template <typename T>
    QueueInserter<T>::~QueueInserter() noexcept {
        if (nullptr != queue_) {
            queue_->close();
        }
    }
} // namespace web_server::threads::queue

#endif // THREADS_QUEUE_INSERTER_HPP
