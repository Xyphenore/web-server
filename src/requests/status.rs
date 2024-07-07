use std::cmp::Ordering;
use std::fmt::{Debug, Display, Formatter};

/// HTTP status [MDN Status](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status).
///
/// # How to use it?
///
/// ```rust
/// use crate::requests::Status;
///
/// let ok = Status::OK;
/// let not_found = Status::NOT_FOUND;
///
/// // You use it like an enumeration.
/// ```
#[derive(Debug, Clone)]
pub struct Status {
    #[doc(hidden)]
    code: u16,
    #[doc(hidden)]
    name: &'static str,
}

impl PartialOrd for Status {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        self.code.partial_cmp(&other.code)
    }

    fn lt(&self, other: &Self) -> bool {
        self.code < other.code
    }

    fn le(&self, other: &Self) -> bool {
        self.code <= other.code
    }

    fn gt(&self, other: &Self) -> bool {
        self.code > other.code
    }

    fn ge(&self, other: &Self) -> bool {
        self.code >= other.code
    }
}

impl PartialEq for Status {
    fn eq(&self, other: &Self) -> bool {
        self.code == other.code
    }
}

impl Display for Status {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "{} {}", self.code, self.name)
    }
}

impl Status {
    /// HTTP status `OK`.
    ///
    /// [MDN - 200 OK](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/200)
    pub const OK: &'static Status = &Self {
        code: 200,
        name: "OK",
    };

    /// HTTP status `NOT FOUND`.
    ///
    /// [MDN - 404 NOT FOUND](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/404)
    pub const NOT_FOUND: &'static Status = &Self {
        code: 404,
        name: "NOT FOUND",
    };
}
