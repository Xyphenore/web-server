#ifndef THREADS_WORKER_HPP
#define THREADS_WORKER_HPP

#include <thread>
#include <iostream>
#include <sstream>

#include <web-server/requests/job.hpp>
#include "queue/extractor.hpp"
#include "queue/queue.hpp"


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
    class Worker final {
        public:
            Worker() = delete;

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
            Worker(std::size_t id, queue::QueueExtractor<requests::Job> queue) :
                handle_{serve, id, queue} {}

            /// Join the thread and wait until the thread ends its execution.
            ///
            /// # Returns
            ///
            /// Returns the error of the thread or nothing if all is good.
            void join() {
                handle_.join();
            }

        private:
            static void serve(const std::size_t id, queue::QueueExtractor<requests::Job> queue) {
                auto is_running = true;

                do {
                    try {
                        auto job = queue.pop();
                        job.execute().send();
                    }
                    catch (const queue::errors::QueueClosedException&) {
                        std::ostringstream msg{};
                        msg << id;

                        std::cout << "Worker " << msg.str() << " disconnected; shutting down.\n";

                        is_running = false;
                    }
                }
                while (is_running);
            }

            std::thread handle_;
    };
} // namespace web_server::threads

#endif // THREADS_WORKER_HPP
