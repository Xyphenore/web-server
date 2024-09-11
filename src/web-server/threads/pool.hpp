#ifndef THREADS_POOL_HPP
#define THREADS_POOL_HPP

#include "queue/inserter.hpp"
#include "worker.hpp"

#include <array>
#include <cstddef>
#include <utility>

namespace web_server::threads {
    using WorkerAmount = std::size_t;

    constexpr WorkerAmount DEFAULT_AMOUNT{1};

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
    template <WorkerAmount amount = DEFAULT_AMOUNT>
    class [[nodiscard]] WorkerPool final {
            static constexpr WorkerAmount MIN_AMOUNT{1};

            static_assert(MIN_AMOUNT <= amount, "The given value is '0'. Please give an integer greater than 0.");

        public:
            using Job = Worker::Job;

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
            constexpr WorkerPool() noexcept;

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
            void execute(Job&& job) noexcept;

        private:
            using QueueInserter = queue::QueueInserter<Job>;
            using QueueExtractor = Worker::QueueExtractor;

            using WorkerID = Worker::ID;

            template <WorkerID... ids>
            using WorkerIDs = std::integer_sequence<WorkerID, ids...>;

            template <WorkerID... ids>
            explicit WorkerPool(QueueInserter queue, WorkerIDs<ids...> ids_) noexcept;

            std::array<Worker, amount> workers_;
            QueueInserter queue_;
    };
} // namespace web_server::threads

namespace web_server::threads {
    template <WorkerAmount amount>
    WorkerPool<amount>::WorkerPool() noexcept:
    WorkerPool{QueueInserter{}, std::make_integer_sequence<WorkerID, amount>{}} {}

    template <WorkerAmount amount>
    void WorkerPool<amount>::execute(Job&& job) noexcept {
        queue_.push(std::move(job));
    }

    template <WorkerAmount amount>
    template <WorkerPool<>::WorkerID... ids>
    WorkerPool<amount>::WorkerPool(QueueInserter queue, [[maybe_unused]] WorkerIDs<ids...> ids_) noexcept:
    workers_{Worker{ids, queue.make_extractor()}...}, queue_{std::move(queue)} {}
} // namespace web_server::threads

#endif // THREADS_POOL_HPP
