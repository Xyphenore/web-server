use std::fmt::{Debug, Display, Formatter};

/// HTTP status [MDN Status](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status).
///
/// # How to use it?
///
/// ```rust
/// use crate::requests::Status;
///
/// let ok = Status::Ok;
/// let not_found = Status::NotFound;
///
/// // You use it like an enumeration.
/// ```
#[derive(Debug, Clone, Hash, PartialEq, Eq, Ord, PartialOrd, Copy)]
pub enum Status {
    /// HTTP status `OK`.
    ///
    /// [MDN - 200 OK](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/200)
    Ok,

    /// HTTP status `NOT FOUND`.
    ///
    /// [MDN - 404 NOT FOUND](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/404)
    NotFound,
}

impl Display for Status {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let (code, name) = match self {
            Self::Ok => (200, "OK"),
            Self::NotFound => (404, "NOT FOUND"),
        };

        write!(f, "{} {}", code, name)
    }
}
