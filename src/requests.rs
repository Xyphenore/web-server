pub use self::job::Job;
pub use self::method::Method;
pub use self::request::Request;
pub use self::response::Response;
pub use self::status::Status;
use self::version::Version;

/// Type for functions that can process a [`Request`] and returns a [`Response`].
///
/// # Examples
///
/// Check examples of [`WebServer`](crate::server::WebServer).
pub type HTTPListener = fn(Request) -> Response;

/// Module contains the [`Job`] structure.
mod job;
mod listener;
mod method;
mod request;
mod response;

/// Module contains the HTTP [`Status`].
mod status;

/// Module contains the HTTP [`Version`].
///
/// # Errors
///
/// - [`InvalidHTTPVersionError`](version::InvalidHTTPVersionError): Indicate that
/// [`Version::from()`] reads an invalid HTTP version.
mod version;
