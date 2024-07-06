use std::fmt::{Display, Formatter};

use super::worker::Worker;

#[derive(Debug)]
pub struct WorkerPool {
    workers: Vec<Worker>,
}

impl WorkerPool {
    /// Create a new ThreadPool.
    ///
    /// The capacity is the number of threads in the pool.
    ///
    /// # Panics
    ///
    /// The `new` function will panic if the size is zero.
    pub fn new(capacity: usize) -> Self {
        if capacity == 0 {
            panic!("Pool capacity cannot be zero");
        }

        let mut workers = Vec::with_capacity(capacity);
        for id in 0..capacity {
            workers.push(Worker::new(id));
        }

        Self { workers }
    }

    pub fn execute(
        &mut self,
        executor: impl FnOnce() + Send + 'static,
    ) -> Result<(), NoAvailableWorkerError> {
        let mut has_executed = false;

        for worker in self.workers.iter_mut() {
            if worker.is_available() {
                worker.execute(executor);
                has_executed = true;
                break;
            }
        }

        if !has_executed {
            return Err(NoAvailableWorkerError::new());
        }

        Ok(())
    }

    pub fn is_any_available(&self) -> bool {
        self.workers.iter().any(|worker| worker.is_available())
    }
}

#[derive(Debug, Clone)]
pub struct NoAvailableWorkerError {}

impl Display for NoAvailableWorkerError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Any worker is available in the pool.")
    }
}

impl NoAvailableWorkerError {
    fn new() -> Self {
        Self {}
    }
}
