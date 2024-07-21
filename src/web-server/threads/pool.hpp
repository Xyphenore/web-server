#ifndef THREADS_POOL_HPP
#define THREADS_POOL_HPP

#include <cstddef>
#include <vector>
#include <stdexcept>
#include <utility>

#include <web-server/requests/job.hpp>
#include <web-server/tools.hpp>
#include "worker.hpp"
#include "queue/creator.hpp"
#include "queue/inserter.hpp"

namespace web_server::threads {
    namespace errors {
        class InvalidWorkerAmountError final: public std::invalid_argument {
            public:
                InvalidWorkerAmountError() noexcept:
                    std::invalid_argument{"The worker amount is zero."} {}
        };
    } // namespace errors

    /// A pool of workers to execute multiple [`Job`]s in parallel.
    ///
    /// # How to create it?
    ///
    /// ```rust
    /// // Logic in the server in `src/server.rs`.
    ///
    /// use crate::threads::WorkerPool;
    ///
    /// // Create a pool of 5 workers.
    /// let workers = WorkerPool::new(5);
    ///
    /// // Now, the pool waits a job.
    /// ```
    ///
    /// # How to stop it?
    ///
    /// To stop the pool, just drop it.
    ///
    /// <!-- References -->
    ///
    /// [WorkerPool]: WorkerPool
    class WorkerPool final {
        public:
            WorkerPool() :
                WorkerPool{DEFAULT_AMOUNT, queue::make_queue<requests::Job>()} {}

            /// Create a new WorkerPool.
            ///
            /// # Parameters
            ///
            /// - `capacity`: The number of threads in the pool.
            ///
            /// # Returns
            ///
            /// Returns a new instance of [`WorkerPool`].
            ///
            /// # Panics
            ///
            /// - If the size is zero.
            explicit WorkerPool(const tools::NonZeroSizeT amount_workers):
                WorkerPool{
                    static_cast<std::size_t>(amount_workers),
                    queue::make_queue<requests::Job>()
                } {}

            WorkerPool(const WorkerPool& other) = delete;

            WorkerPool(WorkerPool&& other) noexcept:
                workers_{std::move(other.workers_)}, queue_{std::move(other.queue_)} {}

            ~WorkerPool() noexcept {
                try {
                    queue_.close();
                }
                catch (const queue::errors::QueueClosedException&) {
                    // Do nothing if the queue is already closed.
                }

                while (!workers_.empty()) {
                    workers_.back().join();
                    workers_.pop_back();
                }
            }

            WorkerPool& operator=(const WorkerPool& other) = delete;

            WorkerPool& operator=(WorkerPool&& other) noexcept {
                if (this != &other) {
                    this->queue_ = std::move(other.queue_);
                    this->workers_ = std::move(other.workers_);
                }

                return *this;
            }

            /// Execute a [`Job`] in any worker.
            ///
            /// # Parameters
            ///
            /// - `job`: The [`Job`] to execute.
            /// If any worker is available, the job is stored in the
            /// [`std::sync::mpsc::channel()`] and the `job` waits that a worker is available.
            ///
            /// # Returns
            ///
            /// Returns a [`SendError`] if the queue cannot accept the [`Job`], else returns
            /// nothing if all is good.
            ///
            /// # Panics
            ///
            /// - If the pool is used during its drop.
            void execute(requests::Job job) noexcept {
                queue_.push(std::move(job));
            }

            void close() {
                try {
                    queue_.close();
                }
                catch (const queue::errors::QueueClosedException&) {
                    // Do nothing if the queue is already closed.
                }

                while (!workers_.empty()) {
                    workers_.back().join();
                    workers_.pop_back();
                }
            }

        private:
            constexpr static auto DEFAULT_AMOUNT = static_cast<std::size_t>(1);

            WorkerPool(
                const std::size_t amount,
                const std::pair<queue::QueueInserter<requests::Job>, queue::QueueExtractor<requests::Job>>& parts
                ):
                queue_{parts.first} {
                if (static_cast<std::size_t>(0) == amount) {
                    throw errors::InvalidWorkerAmountError{};
                }

                workers_.reserve(amount);

                for (auto id = static_cast<std::size_t>(0); id < amount; ++id) {
                    std::ignore = workers_.emplace_back(std::size_t{id}, parts.second);
                }
            }

            std::vector<Worker> workers_{};
            queue::QueueInserter<requests::Job> queue_;
    };
} // namespace web_server::threads

#endif // THREADS_POOL_HPP
