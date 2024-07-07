use std::io::ErrorKind::WouldBlock;
use std::net::TcpListener;
use std::sync::{Arc, Mutex};

pub use crate::requests::{Debug, Method};
use crate::requests::{HTTPListener, RequestHandler};

pub struct WebServer {
    handler: RequestHandler,
}

impl WebServer {
    pub fn new(amount_threads: usize, debug: Debug) -> Self {
        Self {
            handler: RequestHandler::new(amount_threads, debug),
        }
    }

    pub fn add_listener(&mut self, method: Method, listener: HTTPListener) -> &mut Self {
        self.handler.add_listener(method, listener);
        self
    }

    pub fn serve(&mut self) {
        let listener = TcpListener::bind("127.0.0.1:8000").unwrap();
        listener
            .set_nonblocking(true)
            .expect("Cannot make the TCP listener to non-blocking mode.");

        let is_running = Arc::new(Mutex::new(true));
        println!(
            "Server started and waiting for incoming connections on {}.",
            listener.local_addr().unwrap()
        );

        let cloned_is_running = Arc::clone(&is_running);
        ctrlc::set_handler(move || {
            *(cloned_is_running.lock().expect("Want to lock 'is_running'")) = false
        })
        .expect("Cannot set handler for ctrl+c");

        while *(is_running.lock().expect("Cannot lock 'is_running'")) {
            let stream = listener.accept();

            match stream {
                Err(ref e) if e.kind() == WouldBlock => {}
                Ok((stream, _)) => {
                    stream
                        .set_nonblocking(false)
                        .expect("Cannot make the TCP stream to blocking mode.");
                    self.handler.handle(stream);
                }
                Err(error) => panic!("encountered IO error: {}", error),
            }
        }
    }
}
