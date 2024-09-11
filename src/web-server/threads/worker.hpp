#ifndef THREADS_WORKER_HPP
#define THREADS_WORKER_HPP

#include "queue/extractor.hpp"

#include <web-server/helpers/threads.hpp>
#include <web-server/requests/job.hpp>

#include <cstddef>

namespace web_server::threads {
    /// Abstraction layer around a [`JoinHandle`].
    ///
    /// <div class="warning">
    ///
    /// You don't need to create it directly, it is the role of
    /// [`WorkerPool`][WorkerPool].
    ///
    /// </div>
    ///
    /// # How to create it?
    ///
    /// ```rust
    /// // Logic in the pool in `src/threads/pool.rs`.
    ///
    /// use std::sync::{Arc, Mutex};
    /// use std::sync::mpsc::channel;
    ///
    /// use crate::threads::worker::Worker;
    ///
    /// let (tx, rx) = channel();
    /// let rx = Arc::new(Mutex::new(rx));
    ///
    /// let worker = Worker::new(0, Arc::clone(&rx));
    /// // Now, the worker waits a job.
    /// ```
    ///
    /// The created thread waits until a job is sent by [`WorkerPool`][WorkerPool].
    ///
    /// # How to stop it?
    ///
    /// To stop the worker execution, just drop the emitter/sender (tx) of the
    /// [`std::sync::mpsc::channel()`], like `drop(tx)`.
    ///
    /// ```rust
    /// // Logic in the pool in `src/threads/pool.rs`.
    ///
    /// use std::sync::{Arc, Mutex};
    /// use std::sync::mpsc::channel;
    ///
    /// use crate::threads::worker::Worker;
    ///
    /// let (tx, rx) = channel();
    /// let rx = Arc::new(Mutex::new(rx));
    ///
    /// let worker = Worker::new(0, Arc::clone(&rx));
    ///
    /// // To stop the worker
    /// drop(tx);
    /// worker.join().unwrap();
    /// ```
    ///
    /// # About the worker name
    ///
    /// The created thread has a name like: `Worker-{id}`, where `{id}` is
    /// replaced by the given ID in [`Worker::new()`].
    ///
    /// <!-- References -->
    ///
    /// [WorkerPool]: super::pool::WorkerPool
    class [[nodiscard]] Worker final {
        public:
            using Job = requests::Job;
            using QueueExtractor = queue::QueueExtractor<Job>;

            using ID = std::size_t;

            /// Create a new worker with the ID and the receiver part of the
            /// [`std::sync::mpsc::channel()`] aka `queue`.
            ///
            /// # Parameters
            ///
            /// - `id`: ID given by the [`WorkerPool`][WorkerPool].
            /// - `queue`: The receiver (rx) part of the channel created by the
            /// [`WorkerPool`][WorkerPool].
            ///
            /// # Returns
            ///
            /// Returns a new instance of [`Worker`].
            ///
            /// # Panics
            ///
            /// - If the spawn of the created thread returns an error.
            ///
            /// <!-- References -->
            ///
            /// [WorkerPool]: super::pool::WorkerPool
            explicit Worker(ID id, QueueExtractor queue) noexcept;

        private:
            friend void swap(Worker& lhs, Worker& rhs) noexcept { std::swap(lhs.handle_, rhs.handle_); }

            [[nodiscard]] friend bool operator==(const Worker& lhs, const Worker& rhs) noexcept {
                return lhs.handle_ == rhs.handle_;
            }

            [[nodiscard]] friend bool operator!=(const Worker& lhs, const Worker& rhs) noexcept {
                return not(lhs == rhs);
            }

            helpers::Thread handle_;
    };
} // namespace web_server::threads

#endif // THREADS_WORKER_HPP
