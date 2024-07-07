use std::sync::mpsc::{channel, SendError, Sender};
use std::sync::{Arc, Mutex};

use crate::requests::Job;

use super::worker::Worker;

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
#[derive(Debug)]
pub struct WorkerPool {
    #[doc(hidden)]
    workers: Vec<Worker>,
    #[doc(hidden)]
    queue: Option<Sender<Job>>,
}

impl Drop for WorkerPool {
    fn drop(&mut self) {
        drop(self.queue.take());

        while !self.workers.is_empty() {
            self.workers.remove(0).join().unwrap();
        }
    }
}

impl WorkerPool {
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
    pub fn new(capacity: usize) -> WorkerPool {
        if capacity == 0 {
            panic!("Pool capacity cannot be zero");
        }

        let (sender, receiver) = channel();
        let receiver = Arc::new(Mutex::new(receiver));

        let mut workers = Vec::with_capacity(capacity);
        for id in 0..capacity {
            workers.push(Worker::new(id, Arc::clone(&receiver)));
        }

        Self {
            workers,
            queue: Some(sender),
        }
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
    pub fn execute(&mut self, job: Job) -> Result<(), SendError<Job>> {
        self.queue.as_ref().unwrap().send(job)
    }
}
