use std::cmp::Ordering;
use std::fmt::{Display, Formatter};
use std::ops::Deref;
use std::string::ToString;

#[derive(Debug, Clone)]
pub struct Status {
    code: u32,
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
    pub const OK: Self = Self {
        code: 200,
        name: "OK".to_string(),
    };
    pub const NOT_FOUND: Self = Self {
        code: 404,
        name: "NOT FOUND".to_string(),
    };
    pub const INTERNAL_ERROR: Self = Self {
        code: 500,
        name: "INTERNAL ERROR".to_string(),
    };

    const ALLOWED_STATUS: Vec<&'static Self> =
        vec![&Self::OK, &Self::NOT_FOUND, &Self::INTERNAL_ERROR];

    pub fn code(&self) -> u32 {
        self.code
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn from(value: &str) -> Result<&'static Self, InvalidHTTPFullNameStatusError> {
        let upper_value = value.to_uppercase();

        Self::ALLOWED_STATUS
            .iter()
            .find(|status| status.to_string() == upper_value)
            .and_then(|status| Some(status.deref()))
            .ok_or(InvalidHTTPFullNameStatusError::new(value))
    }

    pub fn from_code(value: u32) -> Result<&'static Self, InvalidHTTPCodeStatusError> {
        let string_value = value.to_string();

        Self::ALLOWED_STATUS
            .iter()
            .find(|status| {
                let status = status.to_string().split(" ").next().unwrap();
                status == string_value
            })
            .and_then(|status| Some(status.deref()))
            .ok_or(InvalidHTTPCodeStatusError::new(&string_value))
    }

    pub fn from_name(value: &str) -> Result<&'static Self, InvalidHTTPNameStatusError> {
        let upper_value = value.to_uppercase();

        Self::ALLOWED_STATUS
            .iter()
            .find(|status| {
                let mut parts = status.to_string().split(" ");
                parts.next();
                upper_value == String::from_iter(parts.collect())
            })
            .and_then(|status| Some(status.deref()))
            .ok_or(InvalidHTTPNameStatusError::new(value))
    }
}

pub trait InvalidHTTPStatusError {
    fn new(entry: &str) -> Self;
}

#[derive(Debug, Clone)]
pub struct InvalidHTTPFullNameStatusError {
    entry: String,
}

#[derive(Debug, Clone)]
pub struct InvalidHTTPCodeStatusError {
    entry: String,
}

#[derive(Debug, Clone)]
pub struct InvalidHTTPNameStatusError {
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

impl InvalidHTTPStatusError for InvalidHTTPFullNameStatusError {
    fn new(entry: &str) -> Self {
        Self {
            entry: entry.to_string(),
        }
    }
}

impl InvalidHTTPStatusError for InvalidHTTPCodeStatusError {
    fn new(entry: &str) -> Self {
        Self {
            entry: entry.to_string(),
        }
    }
}

impl InvalidHTTPStatusError for InvalidHTTPNameStatusError {
    fn new(entry: &str) -> Self {
        Self {
            entry: entry.to_string(),
        }
    }
}
