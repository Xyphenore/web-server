// https://doc.rust-lang.org/book/ch20-00-final-project-a-web-server.html

use std::net::TcpListener;

fn main() {
    let listener = TcpListener::bind("127.0.0.1:8000").unwrap();

    println!("Server started, waiting incoming connections.");

    for stream in listener.incoming() {
        let stream = stream.unwrap();

        println!(
            "Connection established with: {}",
            stream.peer_addr().unwrap()
        );
    }
}
