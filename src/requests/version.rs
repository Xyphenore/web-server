use std::fmt::{Display, Formatter};
use std::ops::Deref;

#[derive(Debug, Clone, PartialEq, PartialOrd, Eq, Ord, Hash)]
pub struct Version {
    major: char,
    minor: char,
}

impl Display for Version {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        if '0' == self.minor {
            write!(f, "HTTP/{}.{}", self.major, self.minor)
        } else {
            write!(f, "HTTP/{}", self.major)
        }
    }
}

impl Version {
    pub const HTTP1: Self = Self {
        major: '1',
        minor: '0',
    };
    pub const HTTP1_1: Self = Self {
        major: '1',
        minor: '1',
    };
    pub const HTTP2: Self = Self {
        major: '2',
        minor: '0',
    };
    pub const HTTP3: Self = Self {
        major: '3',
        minor: '0',
    };

    const ALLOWED_VERSIONS: Vec<&'static Self> =
        vec![&Self::HTTP1, &Self::HTTP1_1, &Self::HTTP2, &Self::HTTP3];

    /// Get a constant reference to an HTTP version.
    ///
    /// The line must be in the form: HTTP/{version}<br>
    /// {version} can be 'major' or 'major.minor'
    ///
    /// # Examples
    ///
    /// ```
    /// let version = Version::from("HTTP/1");
    /// assert!(version.is_ok());
    /// if let Some(version) = version {
    ///     assert_eq!(version.to_string(), "HTTP/1");
    /// }
    /// ```
    ///
    /// ```
    /// let version = Version::from("1.0");
    /// assert!(version.is_err());
    /// if let Err(error) = version {
    ///     assert_eq!(error, "Invalid HTTP version: '1.0'");
    /// }
    /// ```
    pub fn from(line: impl AsRef<str>) -> Result<&'static Self, InvalidHTTPVersionError> {
        let upper_line = line.as_ref().to_uppercase();

        Self::ALLOWED_VERSIONS
            .iter()
            .find(|version| version.to_string() == upper_line)
            .and_then(|version| Some(version.deref()))
            .ok_or(InvalidHTTPVersionError::new(line))
    }
}

#[derive(Debug, Clone)]
struct InvalidHTTPVersionError {
    entry: String,
}

impl Display for InvalidHTTPVersionError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid HTTP version: '{}'", &self.entry)
    }
}

impl InvalidHTTPVersionError {
    fn new(entry: impl AsRef<str>) -> Self {
        Self {
            entry: entry.as_ref().to_string(),
        }
    }
}
