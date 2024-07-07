use std::sync::mpsc::Receiver;
use std::sync::{Arc, Mutex};
use std::thread::{Builder, JoinHandle};

use crate::requests::Job;

#[derive(Debug)]
pub struct Worker {
    _handle: JoinHandle<()>,
}

impl Worker {
    pub fn new(id: usize, queue: Arc<Mutex<Receiver<Job>>>) -> Self {
        Self {
            _handle: Builder::new()
                .name(format!("Worker - {id}"))
                .spawn(move || loop {
                    let job = queue.lock().unwrap().recv().unwrap();

                    println!("Worker {id} got a job; executing.");

                    job.execute().send();
                })
                .unwrap(),
        }
    }
}
