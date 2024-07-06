use std::net::TcpListener;

use crate::requests::{Debug, HTTPListener, Method, RequestHandler};

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

        println!(
            "Server started and waiting for incoming connections on {}.",
            listener.local_addr().unwrap()
        );

        loop {
            listener
                .accept()
                .and_then(|stream| Ok(self.handler.handle(stream.0)))
                .unwrap();
            self.handler.process_waiting_request();
        }
    }
}
