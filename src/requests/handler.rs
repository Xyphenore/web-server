use std::collections::HashMap;
use std::net::TcpStream;
use std::path::Path;

use crate::threads::WorkerPool;

use super::{Method, Request, Response, Status};

pub type HTTPListener = fn(Request) -> Response;

#[derive(Debug)]
pub struct RequestHandler {
    cpt: usize,
    debug: bool,
    listeners: HashMap<Method, HTTPListener>,
    workers: WorkerPool,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Debug {
    True,
    False,
}

impl RequestHandler {
    pub fn new(amount_threads: usize, debug: Debug) -> RequestHandler {
        RequestHandler {
            cpt: 0,
            debug: debug == Debug::True,
            listeners: HashMap::new(),
            workers: WorkerPool::new(amount_threads),
        }
    }

    pub fn add_listener(&mut self, method: Method, listener: HTTPListener) {
        if self.listeners.contains_key(&method) {
            panic!("A listener is always registered for {}", method)
        }

        self.listeners.insert(method, listener);
    }

    pub fn handle(&mut self, stream: TcpStream) {
        let request = Request::from_stream(stream);

        if self.debug {
            println!("Request {}: {request:#?}", self.cpt);
        }
        self.cpt += 1;

        let listener = self
            .listeners
            .get(&request.method)
            .unwrap_or(&(RequestHandler::not_found_handler as HTTPListener));

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
