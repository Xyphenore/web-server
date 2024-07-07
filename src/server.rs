use std::io;
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
        listener
            .set_nonblocking(true)
            .expect("Cannot make the TCP listener to non-blocking mode.");

        println!(
            "Server started and waiting for incoming connections on {}.",
            listener.local_addr().unwrap()
        );

        loop {
            let stream = listener.accept();

            match stream {
                Err(ref e) if e.kind() == io::ErrorKind::WouldBlock => {}
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
