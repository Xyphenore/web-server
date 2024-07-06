use std::collections::HashMap;
use std::io::{BufRead, BufReader, Write};
use std::net::TcpStream;
use std::path::Path;

use crate::threads::WorkerPool;

use super::{Method, Request, Response, Status, Version};

type HTTPListener = fn(Request) -> Response;

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

    pub fn remove_listener(&mut self, method: Method) {
        if self.listeners.remove(&method) == None {
            panic!("Any listener is registered for {}", method)
        }
    }

    pub fn handle(&mut self, mut stream: TcpStream) {
        let buffer_reader = BufReader::new(&stream);
        let mut http_request: Vec<_> = buffer_reader
            .lines()
            .map(|result| result.unwrap())
            .take_while(|line| !line.is_empty())
            .collect();

        if self.debug {
            println!("Request {}: {http_request:#?}", self.cpt);
        }
        self.cpt += 1;

        let first_line = http_request.remove(0);
        let method = Method::from_line(&first_line).unwrap();

        let mut parts = first_line.split(" ");
        // Drop the method verb
        parts.next();
        // Drop the URI
        parts.next();

        let request = Request {
            method,
            version: Version::from(String::from_iter(parts)).unwrap(),
            other_lines: http_request,
        };

        let listener = self
            .listeners
            .get(&request.method)
            .unwrap_or(&(RequestHandler::not_found_handler as HTTPListener))
            .clone();

        if self.workers.is_any_available() {
            self.workers
                .execute(move || {
                    let response = listener(request);
                    stream.write_all(response.to_string().as_bytes()).unwrap();
                })
                .unwrap();
        } else {
            let response = Self::service_unavailable(request);
            stream.write_all(response.to_string().as_bytes()).unwrap();
        }
    }

    fn not_found_handler(request: Request) -> Response {
        let mut response = Response::new(request.version, Status::NOT_FOUND);
        response
            .add_file(Path::new("templates/not_found.html"))
            .unwrap();

        response
    }

    fn service_unavailable(request: Request) -> Response {
        let mut response = Response::new(request.version, Status::SERVICE_UNAVAILABLE);
        response
            .add_file(Path::new("templates/service_unavailable.html"))
            .unwrap();

        response
    }
}
