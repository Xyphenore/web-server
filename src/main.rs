// https://doc.rust-lang.org/book/ch20-00-final-project-a-web-server.ht

use std::net::TcpListener;

use crate::requests::{Method, RequestHandler};
use crate::routes::index::get as get_index;
use crate::routes::slow_request::get as get_slow_request;

mod requests;
mod routes;

static DEBUG: bool = false;

fn main() {
    let listener = TcpListener::bind("127.0.0.1:8000").unwrap();

    println!(
        "Server started and waiting for incoming connections on {}.",
        listener.local_addr().unwrap()
    );

    let mut handler = if DEBUG {
        RequestHandler::new_with_debug()
    } else {
        RequestHandler::new()
    };

    handler.add_listener(Method::get("/").unwrap(), get_index);
    handler.add_listener(Method::get("/slow_request").unwrap(), get_slow_request);

    for stream in listener.incoming() {
        handler.handle(stream.unwrap());
    }
}
