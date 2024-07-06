use std::collections::HashMap;
use std::io::{BufRead, BufReader, Write};
use std::net::TcpStream;
use std::path::Path;

use super::{Method, Request, Response, Status, Version};

type HTTPListener = fn(Request) -> Response;

pub struct RequestHandler {
    cpt: u32,
    debug: bool,
    listeners: HashMap<Method, HTTPListener>,
}

impl RequestHandler {
    pub fn new() -> RequestHandler {
        RequestHandler {
            cpt: 0,
            debug: false,
            listeners: HashMap::new(),
        }
    }

    pub fn new_with_debug() -> RequestHandler {
        RequestHandler {
            cpt: 0,
            debug: true,
            listeners: HashMap::new(),
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
            .unwrap_or(&(RequestHandler::not_found_handler as HTTPListener));

        let response = listener(request);
        stream.write_all(response.to_string().as_bytes()).unwrap();
    }

    fn not_found_handler(request: Request) -> Response {
        let mut response = Response::new(request.version, Status::NOT_FOUND);
        response
            .add_file(Path::new("templates/not_found.html"))
            .unwrap();

        response
    }
}
