use std::any::Any;
use std::sync::mpsc::Receiver;
use std::sync::{Arc, Mutex};
use std::thread::{Builder, JoinHandle};

use crate::requests::Job;

#[derive(Debug)]
pub struct Worker {
    handle: JoinHandle<()>,
}

impl Worker {
    pub fn new(id: usize, queue: Arc<Mutex<Receiver<Job>>>) -> Self {
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

    pub fn join(self) -> Result<(), Box<dyn Any + Send + 'static>> {
        self.handle.join()
    }
}
