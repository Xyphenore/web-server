use std::sync::mpsc::{channel, SendError, Sender};
use std::sync::{Arc, Mutex};

use crate::requests::Job;

use super::worker::Worker;

#[derive(Debug)]
pub struct WorkerPool {
    workers: Vec<Worker>,
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
            workers,
            queue: Some(sender),
        }
    }

    pub fn execute(&mut self, job: Job) -> Result<(), SendError<Job>> {
        self.queue.as_ref().unwrap().send(job)
    }
}
