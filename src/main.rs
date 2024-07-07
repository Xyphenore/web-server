// https://doc.rust-lang.org/book/ch20-00-final-project-a-web-server.ht

use crate::requests::{Debug, Method};
use crate::routes::index::get as get_index;
use crate::routes::slow_request::get as get_slow_request;
use crate::server::WebServer;

mod requests;
mod routes;
mod server;
mod threads;

static DEBUG: bool = false;

fn main() {
    WebServer::new(2, if DEBUG { Debug::True } else { Debug::False })
        .add_listener(Method::get("/").unwrap(), get_index)
        .add_listener(Method::get("/slow_request").unwrap(), get_slow_request)
        .serve();
}
