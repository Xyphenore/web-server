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

/// Module contains the HTTP [`Method`].
///
/// # Errors
///
/// - [`InvalidMethodPartError`](method::InvalidMethodPartError): Indicate that
/// [`Method::try_from()`] reads an invalid part (URI, Method).
/// - [`InvalidMethodError`](method::InvalidMethodError): Indicate that
/// [`Method::try_from()`] reads an invalid method verb.
/// - [`InvalidURIError`](method::InvalidURIError): Indicate that
/// [`Method::try_from()`] reads an invalid URI.
mod method;

/// Module contains the [`Request`] structure.
mod request;

/// Module contains the [`Response`] structure.
mod response;

/// Module contains the HTTP [`Status`].
mod status;

/// Module contains the HTTP [`Version`].
///
/// # Errors
///
/// - [`InvalidHTTPVersionError`](version::InvalidHTTPVersionError): Indicate that
/// [`Version::try_from()`] reads an invalid HTTP version.
mod version;
