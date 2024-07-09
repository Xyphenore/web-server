use std::any::Any;
use std::fmt::{Display, Formatter};
use std::sync::mpsc::Receiver;
use std::sync::{Arc, Mutex};
use std::thread::{Builder, JoinHandle};

use crate::requests::Job;

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
#[derive(Debug)]
pub struct Worker {
    #[doc(hidden)]
    handle: JoinHandle<()>,
}

impl Worker {
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
    pub fn new(id: usize, queue: Arc<Mutex<Receiver<Job>>>) -> Worker {
        Self {
            handle: Builder::new()
                .name(format!("Worker - {id}"))
                .spawn(move || loop {
                    let job = queue.lock().unwrap().recv();

                    if let Ok(job) = job {
                        println!("Worker {id} got a job; executing.");
                        job.execute().send();
                    } else {
                        println!("Worker {id} disconnected; shutting down.");
                        break;
                    }
                })
                .unwrap(),
        }
    }

    /// Join the thread and wait until the thread ends its execution.
    ///
    /// # Returns
    ///
    /// Returns the error of the thread or nothing if all is good.
    pub fn join(self) -> Result<(), Box<dyn Any + Send + 'static>> {
        self.handle.join()
    }
}

impl Display for Worker {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "{}",
            self.handle.thread().name().unwrap_or("Worker - unknown")
        )
    }
}
