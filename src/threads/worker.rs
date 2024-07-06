use std::thread::{Builder, JoinHandle};

#[derive(Debug)]
pub struct Worker {
    id: usize,
    handle: Option<JoinHandle<()>>,
}

impl Worker {
    pub fn new(id: usize) -> Self {
        Self { id, handle: None }
    }

    pub fn execute(&mut self, executor: impl FnOnce() + Send + 'static) {
        if !self.is_available() {
            panic!(
                "Cannot execute the executor of the Worker-{} because it is busy.",
                self.id,
            )
        }

        self.handle = None;
        self.handle = Some(
            Builder::new()
                .name(format!("Worker - {}", self.id))
                .spawn(executor)
                .unwrap(),
        );
    }

    pub fn is_available(&self) -> bool {
        self.handle
            .as_ref()
            .map_or(true, |handle| handle.is_finished())
    }
}
