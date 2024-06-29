use std::fmt::{Display, Formatter};
use std::ops::Deref;

#[derive(Debug, Clone, PartialEq, PartialOrd, Eq, Ord)]
pub struct Version {
    code: String,
}

impl Display for Version {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "HTTP/{}", self.code)
    }
}

impl Version {
    pub const HTTP1: Self = Self {
        code: 1.to_string(),
    };
    pub const HTTP1_1: Self = Self {
        code: 1.1.to_string(),
    };
    pub const HTTP2: Self = Self {
        code: 2.to_string(),
    };
    pub const HTTP3: Self = Self {
        code: 3.to_string(),
    };

    const ALLOWED_VERSIONS: Vec<&'_ Self> =
        vec![&Self::HTTP1, &Self::HTTP1_1, &Self::HTTP2, &Self::HTTP3];

    pub fn from(value: &str) -> Result<&Self, InvalidHTTPVersionError> {
        let upper_value = value.to_uppercase();

        Self::ALLOWED_VERSIONS
            .iter()
            .find(|version| version.to_string() == upper_value)
            .and_then(|version| Some(version.deref()))
            .ok_or(InvalidHTTPVersionError::new(value))
    }
}

#[derive(Debug, Clone)]
pub struct InvalidHTTPVersionError {
    entry: String,
}

impl Display for InvalidHTTPVersionError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid HTTP version: '{}'", self.entry)
    }
}

impl InvalidHTTPVersionError {
    pub fn new(entry: &str) -> Self {
        Self {
            entry: entry.to_string(),
        }
    }
}
