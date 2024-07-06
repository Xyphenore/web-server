use std::cmp::Ordering;
use std::fmt::{Debug, Display, Formatter};
use std::ops::Deref;
use std::string::ToString;

#[derive(Debug, Clone, Hash)]
pub struct Status {
    code: u16,
    name: String,
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

    fn ne(&self, other: &Self) -> bool {
        self.code != other.code
    }
}

impl Display for Status {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "{} {}", self.code, self.name)
    }
}

impl Status {
    pub const OK: &'static Self = &Self {
        code: 200,
        name: "OK".to_string(),
    };
    pub const NOT_FOUND: &'static Self = &Self {
        code: 404,
        name: "NOT FOUND".to_string(),
    };
    pub const INTERNAL_ERROR: &'static Self = &Self {
        code: 500,
        name: "INTERNAL ERROR".to_string(),
    };

    const ALLOWED_STATUS: Vec<&'static Self> =
        vec![&Self::OK, &Self::NOT_FOUND, &Self::INTERNAL_ERROR];

    /// Get a reference to a defined constant status from a line of HTTP request.
    ///
    /// The line must be formatted like: `code name`
    ///
    /// # Examples
    ///
    /// ```
    /// let status = Status::from_line("200 OK");
    /// assert!(status.is_ok());
    /// if let Ok(status) = status {
    ///     assert_eq!(status.to_string(), "200 OK");
    /// }
    /// ```
    ///
    /// ```
    /// let status = Status::from_line("200");
    /// assert!(status.is_err());
    /// ```
    pub fn from_line(
        line: impl AsRef<str>,
    ) -> Result<&'static Self, InvalidHTTPFullNameStatusError> {
        let upper_line = line.as_ref().to_uppercase();

        Self::ALLOWED_STATUS
            .iter()
            .find(|status| status.to_string() == upper_line)
            .and_then(|status| Some(status.deref()))
            .ok_or(InvalidHTTPFullNameStatusError::new(line))
    }

    /// Get a reference to a defined constant status from a code.
    ///
    /// # Examples
    ///
    /// ```
    /// let status = Status::from_code(200);
    /// assert!(status.is_ok());
    /// if let Ok(status) = status {
    ///     assert_eq!(status.to_string(), "200 OK");
    /// }
    /// ```
    ///
    /// ```
    /// let status = Status::from_line(10);
    /// assert!(status.is_err());
    /// ```
    pub fn from_code(code: u16) -> Result<&'static Self, InvalidHTTPCodeStatusError> {
        Self::ALLOWED_STATUS
            .iter()
            .find(|status| status.code == code)
            .and_then(|status| Some(status.deref()))
            .ok_or(InvalidHTTPCodeStatusError::new(code.to_string()))
    }

    /// Get a reference to a defined constant status from a status name.
    ///
    /// # Examples
    ///
    /// ```
    /// let status = Status::from_name("OK");
    /// assert!(status.is_ok());
    /// if let Ok(status) = status {
    ///     assert_eq!(status.to_string(), "200 OK");
    /// }
    /// ```
    ///
    /// ```
    /// let status = Status::from_name("200");
    /// assert!(status.is_err());
    /// ```
    pub fn from_name(name: impl AsRef<str>) -> Result<&'static Self, InvalidHTTPNameStatusError> {
        let upper_name = name.as_ref().to_uppercase();

        Self::ALLOWED_STATUS
            .iter()
            .find(|status| upper_name == status.name)
            .and_then(|status| Some(status.deref()))
            .ok_or(InvalidHTTPNameStatusError::new(name))
    }
}

trait InvalidHTTPStatusError: Debug + Clone {}

#[derive(Debug, Clone)]
struct InvalidHTTPFullNameStatusError {
    entry: String,
}

#[derive(Debug, Clone)]
struct InvalidHTTPCodeStatusError {
    entry: String,
}

#[derive(Debug, Clone)]
struct InvalidHTTPNameStatusError {
    entry: String,
}

impl Display for InvalidHTTPFullNameStatusError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid HTTP status: '{}'", self.entry)
    }
}

impl Display for InvalidHTTPCodeStatusError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid HTTP code status: '{}'", self.entry)
    }
}

impl Display for InvalidHTTPNameStatusError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid HTTP name status: '{}'", self.entry)
    }
}

impl InvalidHTTPFullNameStatusError {
    fn new(entry: impl AsRef<str>) -> Self {
        Self {
            entry: entry.as_ref().to_string(),
        }
    }
}

impl InvalidHTTPCodeStatusError {
    fn new(entry: impl AsRef<str>) -> Self {
        Self {
            entry: entry.as_ref().to_string(),
        }
    }
}

impl InvalidHTTPNameStatusError {
    fn new(entry: impl AsRef<str>) -> Self {
        Self {
            entry: entry.as_ref().to_string(),
        }
    }
}

impl InvalidHTTPStatusError for InvalidHTTPFullNameStatusError {}
impl InvalidHTTPStatusError for InvalidHTTPCodeStatusError {}
impl InvalidHTTPStatusError for InvalidHTTPNameStatusError {}
