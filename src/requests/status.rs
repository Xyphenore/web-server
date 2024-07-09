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
#[derive(Debug, Clone, Hash, PartialEq, Eq, Ord, PartialOrd, Copy)]
pub struct Status {
    #[doc(hidden)]
    code: u16,
    #[doc(hidden)]
    name: &'static str,
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

impl Display for Status {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "{} {}", self.code, self.name)
    }
}
