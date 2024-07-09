//! Module providing [`WebServer`], [`enum@Debug`] and [`Method`].
//!
//! To see how to create the web server, go to the class [`WebServer`].

pub use crate::requests::Method;
use crate::requests::{HTTPListener, Job, Request, Response, Status};
use crate::threads::WorkerPool;
use std::collections::HashMap;
use std::io::ErrorKind::WouldBlock;
use std::net::{Ipv4Addr, SocketAddrV4, TcpListener, TcpStream};
use std::num::NonZeroUsize;
use std::path::Path;
use std::sync::{Arc, Mutex};

/// The web server.
///
/// # How to use it?
///
/// ```rust
/// use crate::server::{WebServer, Debug};
/// use crate::requests::{Method, Status, Request, Response};
///
/// fn process(request: Request) -> Response {
///     request.make_response_with_status(Status::OK)
/// }
///
/// let server = WebServer::new(5, Debug::False);
/// server.add_listener(Method::get("/"), process);
///
/// server.serve();
/// ```
#[derive(Debug)]
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
    /// Create the [`WebServer`].
    ///
    /// # Parameters
    ///
    /// - `amount_workers`: The number must be greater than 0, else panics.
    /// - `debug`: Activate the debug mode of the server, cf.[`enum@Debug`].
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`WebServer`].
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::server::{WebServer, Debug};
    ///
    /// let server = WebServer::new(5, Debug::False);
    /// ```
    ///
    /// # Panics
    ///
    /// - If `amount_workers` is equal to 0.
    pub fn new(amount_workers: NonZeroUsize, debug: Debug) -> WebServer {
        Self {
            cpt: 0,
            debug: debug == Debug::True,
            listeners: HashMap::new(),
            workers: WorkerPool::new(amount_workers),
        }
    }

    /// Add the route with the [`Method`] and the [`HTTPListener`].
    ///
    /// # Parameters
    ///
    /// - `method`: The [`Method`] to register.
    /// The `method` must not be yet registered, else panics.
    /// - `listener`: The function to process the incoming request for the `method`.
    ///
    /// # Returns
    ///
    /// Returns the mutable reference to the current instance of [`WebServer`].
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::server::{WebServer, Debug};
    /// use crate::requests::{Method, Status, Request, Response};
    ///
    /// fn process(request: Request) -> Response {
    ///     request.make_response_with_status(Status::OK)
    /// }
    ///
    /// let server = WebServer::new(5, Debug::False);
    /// server.add_listener(Method::get("/"), process);
    /// ```
    ///
    /// # Panics
    ///
    /// - If the `method` is already registered.
    pub fn add_listener(&mut self, method: Method, listener: HTTPListener) -> &mut WebServer {
        assert!(
            !self.listeners.contains_key(&method),
            "A listener is always registered for {}",
            method,
        );

        self.listeners.insert(method, listener);

        self
    }

    /// Execute the server and process incoming requests on `127.0.0.1:8000`.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::server::{WebServer, Debug};
    /// use crate::requests::{Method, Status, Request, Response};
    ///
    /// fn process(request: Request) -> Response {
    ///     request.make_response_with_status(Status::OK)
    /// }
    ///
    /// let server = WebServer::new(5, Debug::False);
    /// server.add_listener(Method::get("/"), process);
    ///
    /// server.serve();
    /// ```
    ///
    /// # Panics
    ///
    /// - If it is not possible to bind the [`TcpListener`] to `127.0.0.1:8000`,
    /// cf.[`TcpListener::bind()`].
    /// - If the [`TcpListener`] cannot enter into the non-blocking mode,
    /// cf.[`TcpListener::set_nonblocking()`].
    /// - If [`TcpListener::local_addr()`] fails.
    /// - If [`ctrlc::set_handler()`] fails.
    /// - If the state `is_running` cannot be locked.
    /// - If the incoming [`TcpStream`] fails.
    /// - If the incoming [`TcpStream`] cannot enter into the blocking mode.
    /// - If the process of the incoming stream, panics.
    pub fn serve(&mut self) {
        let listener = TcpListener::bind("127.0.0.1:8000").unwrap();
        listener
            .set_nonblocking(true)
            .expect("Cannot make the TCP listener to non-blocking mode.");

        let is_running = Arc::new(Mutex::new(true));
        println!(
            "Server started and waiting for incoming connections on {}.",
            listener.local_addr().unwrap(),
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

    /// Process the incoming [`TcpStream`].
    ///
    /// # Panics
    ///
    /// - If the creation of [`Request`] from the stream, panics.
    /// cf.[`Request::from_stream()`].
    /// - If the execution of the request, panics.
    /// cf.[`WorkerPool::execute()`].
    #[doc(hidden)]
    fn handle(&mut self, stream: TcpStream) {
        let request = Request::from(stream);

        if self.debug {
            println!("Request {}: {request:#?}", self.cpt);
        }
        self.cpt += 1;

        let listener = self
            .listeners
            .get(request.method())
            .unwrap_or(&(Self::not_found_handler as HTTPListener));

        self.workers
            .execute(Job {
                request,
                listener: *listener,
            })
            .unwrap();
    }

    /// Process the incoming [`Request`] if any listener is registered for the
    /// [`Method`] contained in the [`Request`].
    ///
    /// # Returns
    ///
    /// Returns the response `404 Not Found` with the `templates/not_found.html`
    /// content.
    ///
    /// # Panics
    ///
    /// - If [`Response::add_file()`] returns an error.
    #[doc(hidden)]
    fn not_found_handler(request: Request) -> Response {
        let mut response = Response::from((request, Status::NOT_FOUND));
        response
            .add_file(Path::new("templates/not_found.html"))
            .unwrap();

        response
    }
}

/// Indicate if the debug mode is activated on the [`WebServer`].
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Debug {
    True,
    False,
}

impl From<bool> for Debug {
    /// Get the [`enum@Debug`] enumeration value from the bool value.
    fn from(value: bool) -> Debug {
        match value {
            true => Self::True,
            false => Self::False,
        }
    }
}
