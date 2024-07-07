use std::collections::HashMap;
use std::io::ErrorKind::WouldBlock;
use std::net::{TcpListener, TcpStream};
use std::path::Path;
use std::sync::{Arc, Mutex};

pub use crate::requests::Method;
use crate::requests::{HTTPListener, Request, Response, Status};
use crate::threads::WorkerPool;

pub struct WebServer {
    #[doc(hidden)]
    cpt: usize,
    #[doc(hidden)]
    debug: bool,
    #[doc(hidden)]
    listeners: HashMap<Method, HTTPListener>,
    #[doc(hidden)]
    workers: WorkerPool,
}

impl WebServer {
    pub fn new(amount_workers: usize, debug: Debug) -> WebServer {
        Self {
            cpt: 0,
            debug: debug == Debug::True,
            listeners: HashMap::new(),
            workers: WorkerPool::new(amount_workers),
        }
    }

    pub fn add_listener(&mut self, method: Method, listener: HTTPListener) -> &mut Self {
        if self.listeners.contains_key(&method) {
            panic!("A listener is always registered for {}", method)
        }

        self.listeners.insert(method, listener);

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
                    self.handle(stream);
                }
                Err(error) => panic!("encountered IO error: {}", error),
            }
        }
    }

    fn handle(&mut self, stream: TcpStream) {
        let request = Request::from_stream(stream);

        if self.debug {
            println!("Request {}: {request:#?}", self.cpt);
        }
        self.cpt += 1;

        let listener = self
            .listeners
            .get(&request.method)
            .unwrap_or(&(WebServer::not_found_handler as HTTPListener));

        self.workers
            .execute(request.make_job_with_listener(*listener))
            .unwrap();
    }

    fn not_found_handler(request: Request) -> Response {
        let mut response = request.make_response_with_status(Status::NOT_FOUND);
        response
            .add_file(Path::new("templates/not_found.html"))
            .unwrap();

        response
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Debug {
    True,
    False,
}

impl Debug {
    pub fn from(value: bool) -> Debug {
        match value {
            true => Self::True,
            false => Self::False,
        }
    }
}
