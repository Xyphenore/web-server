/// The module contains all functions that process requests for the URI `/`.
///
/// # Examples
///
/// Check examples of [`WebServer::add_listener()`][add_listener],
/// to see how to add a route to the server.
///
/// <!-- References -->
///
/// [add_listener]: crate::server::WebServer::add_listener()
pub mod index;

/// The module contains all functions that process requests for the URI `/slow_request`.
///
/// # Examples
///
/// Check examples of [`WebServer::add_listener()`][add_listener],
/// to see how to add a route to the server.
///
/// <!-- References -->
///
/// [add_listener]: crate::server::WebServer::add_listener()
pub mod slow_request;
