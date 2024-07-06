use std::collections::{HashMap, VecDeque};
use std::net::TcpStream;
use std::path::Path;

use crate::threads::WorkerPool;

use super::{Job, Method, Request, Response, Status};

pub type HTTPListener = fn(Request) -> Response;

#[derive(Debug)]
pub struct RequestHandler {
    cpt: usize,
    debug: bool,
    listeners: HashMap<Method, HTTPListener>,
    workers: WorkerPool,
    waiting_jobs: VecDeque<Job>,
    waiting_job_limit: usize,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Debug {
    True,
    False,
}

impl RequestHandler {
    pub fn new(amount_threads: usize, debug: Debug, waiting_job_limit: usize) -> RequestHandler {
        RequestHandler {
            cpt: 0,
            debug: debug == Debug::True,
            listeners: HashMap::new(),
            workers: WorkerPool::new(amount_threads),
            waiting_jobs: VecDeque::new(),
            waiting_job_limit,
        }
    }

    pub fn add_listener(&mut self, method: Method, listener: HTTPListener) {
        if self.listeners.contains_key(&method) {
            panic!("A listener is always registered for {}", method)
        }

        self.listeners.insert(method, listener);
    }

    pub fn remove_listener(&mut self, method: Method) {
        if self.listeners.remove(&method).is_none() {
            panic!("Any listener is registered for {}", method)
        }
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

        if self.workers.is_any_available() {
            let cloned_listener = *listener;
            self.workers
                .execute(move || cloned_listener(request).send())
                .unwrap();
        } else if self.waiting_jobs.len() < self.waiting_job_limit {
            self.waiting_jobs.push_back(Job {
                request,
                listener: *listener,
            });
        } else {
            Self::service_unavailable(request).send();
        }
    }

    pub fn process_waiting_requests(&mut self) {
        while let Some(job) = self.waiting_jobs.pop_front() {
            if self.workers.is_any_available() {
                let listener = job.listener;
                self.workers
                    .execute(move || listener(job.request).send())
                    .unwrap();
            } else {
                self.waiting_jobs.push_front(job);
                break;
            }
        }
    }

    fn not_found_handler(request: Request) -> Response {
        let mut response = request.make_response_with_status(Status::NOT_FOUND);
        response
            .add_file(Path::new("templates/not_found.html"))
            .unwrap();

        response
    }

    fn service_unavailable(request: Request) -> Response {
        let mut response = request.make_response_with_status(Status::SERVICE_UNAVAILABLE);
        response
            .add_file(Path::new("templates/service_unavailable.html"))
            .unwrap();

        response
    }
}
