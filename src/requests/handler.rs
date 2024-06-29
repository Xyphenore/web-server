use std::collections::HashMap;
use std::fs;
use std::io::{BufRead, BufReader, Write};
use std::net::TcpStream;

use super::HTTPMethod;
use super::Request;

pub type HTTPListener = fn(Request) -> String;

pub struct RequestHandler {
    cpt: u32,
    debug: bool,
    listeners: HashMap<String, HTTPListener>,
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

    pub fn add_listener(&mut self, method: HTTPMethod, listener: HTTPListener) {
        if self.listeners.contains_key(&method.to_string()) {
            panic!("A listener is always registered for {}", method.to_string())
        }

        self.listeners
            .insert(method.to_string().to_owned(), listener);
    }

    pub fn remove_listener(&mut self, method: HTTPMethod) {
        if self.listeners.contains_key(&method.to_string()) {
            panic!("Any listener is registered for {}", method.to_string())
        }

        self.listeners.remove(&method.to_string());
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
        let first_line_parts: Vec<_> = first_line.split(" ").collect();

        let request = Request {
            method: first_line_parts[0].to_string(),
            uri: first_line_parts[1].to_string(),
            version: first_line_parts[2].to_string(),
            other_lines: http_request,
        };

        let method_name = HTTPMethod::convert(&request.method, &request.uri);
        let listener = self
            .listeners
            .get(&method_name)
            .unwrap_or(&(RequestHandler::not_found_handler as HTTPListener));

        let response = listener(request);
        stream.write_all(response.as_bytes()).unwrap();
    }

    fn not_found_handler(request: Request) -> String {
        let version = request.version;
        let code = "404 NOT FOUND";

        let response_header = format!("{version} {code}");

        let response_body = fs::read_to_string("templates/not_found.html").unwrap();
        let length = response_body.len();

        format!("{response_header}\r\nContent-Length: {length}\r\n\r\n{response_body}")
    }
}
