use std::sync::mpsc::{channel, SendError, Sender};
use std::sync::{Arc, Mutex};

use crate::requests::Job;

use super::worker::Worker;

#[derive(Debug)]
pub struct WorkerPool {
    _workers: Vec<Worker>,
    queue: Sender<Job>,
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

        let (sender, receiver) = channel();
        let receiver = Arc::new(Mutex::new(receiver));

        let mut workers = Vec::with_capacity(capacity);
        for id in 0..capacity {
            workers.push(Worker::new(id, Arc::clone(&receiver)));
        }

        Self {
            _workers: workers,
            queue: sender,
        }
    }

    pub fn execute(&mut self, job: Job) -> Result<(), SendError<Job>> {
        self.queue.send(job)
    }
}
