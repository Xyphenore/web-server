use std::fmt::{Display, Formatter};
use std::str::FromStr;

/// HTTP version.
///
/// # How to use it?
///
/// ```rust
/// use crate::requests::Version;
///
/// let version_1 = Version::HTTP_1;
/// let version_1_1 = Version::HTTP_1_1;
/// let version_2 = Version::HTTP_2;
/// let version_3 = Version::HTTP_3;
///
/// // You can use it like an enumeration.
/// ```
///
/// ```rust
/// use crate::requests::Version;
///
/// let line = "HTTP/2";
///
/// let version = Version::from(line);
/// ```
#[derive(Debug, Clone, PartialEq, PartialOrd, Eq, Ord, Hash, Copy)]
pub struct Version {
    #[doc(hidden)]
    major: char,
    #[doc(hidden)]
    minor: char,
}

impl Version {
    /// HTTP Version 1
    ///
    /// [MDN - HTTP1](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http1.0_%E2%80%93_building_extensibility)
    pub const HTTP_1: Version = Self {
        major: '1',
        minor: '0',
    };

    /// HTTP Version 1.1
    ///
    /// [MDN - HTTP1.1](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http1.1_%E2%80%93_the_standardized_protocol)
    pub const HTTP_1_1: Version = Self {
        major: '1',
        minor: '1',
    };

    /// HTTP Version 2
    ///
    /// [MDN - HTTP2](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http2_%E2%80%93_a_protocol_for_greater_performance)
    pub const HTTP_2: Version = Self {
        major: '2',
        minor: '0',
    };

    /// HTTP Version 3
    ///
    /// [MDN - HTTP3](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#http3_-_http_over_quic)
    pub const HTTP_3: Version = Self {
        major: '3',
        minor: '0',
    };

    /// Get a constant reference to an HTTP version.
    ///
    /// # Parameters
    ///
    /// - `line`: The line must be in the form: `HTTP/{version}`.
    /// `{version}` can be like `major` or `major.minor`.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns the constant to the good [`Version`], or a
    /// [`InvalidHTTPVersionError`] if `line` does not respect the pattern.
    ///
    /// # Examples
    ///
    /// ```rust
    /// let version = Version::from("HTTP/1");
    /// assert!(version.is_ok());
    ///
    /// if let Some(version) = version {
    ///     assert_eq!(version.to_string(), "HTTP/1");
    /// }
    /// ```
    ///
    /// ```rust
    /// let version = Version::from("1.0");
    /// assert!(version.is_err());
    ///
    /// if let Err(error) = version {
    ///     assert_eq!(error, "Invalid HTTP version: '1.0'");
    /// }
    /// ```
    #[doc(hidden)]
    fn try_from_line(line: impl AsRef<str>) -> Result<Version, InvalidHTTPVersionError> {
        let upper_line = line.as_ref().to_uppercase();

        Self::ALLOWED_VERSIONS
            .iter()
            .find(|version| version.to_string() == upper_line)
            .cloned()
            .ok_or(InvalidHTTPVersionError::from(line.as_ref()))
    }

    /// All allowed versions.
    #[doc(hidden)]
    const ALLOWED_VERSIONS: &'static [Self] =
        &[Self::HTTP_1, Self::HTTP_1_1, Self::HTTP_2, Self::HTTP_3];
}

impl Display for Version {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        if '0' != self.minor {
            write!(f, "HTTP/{}.{}", self.major, self.minor)
        } else {
            write!(f, "HTTP/{}", self.major)
        }
    }
}

impl Default for Version {
    fn default() -> Self {
        Self::HTTP_1_1
    }
}

impl FromStr for Version {
    type Err = InvalidHTTPVersionError;

    /// Get a constant reference to an HTTP version.
    ///
    /// # Parameters
    ///
    /// - `s`: The line must be in the form: `HTTP/{version}`.
    /// `{version}` can be like `major` or `major.minor`.
    ///
    /// # Returns
    ///
    /// Returns the constant to the good [`Version`], or a
    /// [`InvalidHTTPVersionError`] if `line` does not respect the pattern.
    ///
    /// # Examples
    ///
    /// ```rust
    /// let version = Version::from_str("HTTP/1");
    /// assert!(version.is_ok());
    ///
    /// if let Some(version) = version {
    ///     assert_eq!(version.to_string(), "HTTP/1");
    /// }
    /// ```
    ///
    /// ```rust
    /// let version = Version::from_str("1.0");
    /// assert!(version.is_err());
    ///
    /// if let Err(error) = version {
    ///     assert_eq!(error, "Invalid HTTP version: '1.0'");
    /// }
    /// ```
    fn from_str(s: &str) -> Result<Version, Self::Err> {
        Self::try_from(s)
    }
}

impl TryFrom<&str> for Version {
    type Error = InvalidHTTPVersionError;

    /// Get a constant reference to an HTTP version.
    ///
    /// # Parameters
    ///
    /// - `line`: The line must be in the form: `HTTP/{version}`.
    /// `{version}` can be like `major` or `major.minor`.
    ///
    /// # Returns
    ///
    /// Returns the constant to the good [`Version`], or a
    /// [`InvalidHTTPVersionError`] if `line` does not respect the pattern.
    ///
    /// # Examples
    ///
    /// ```rust
    /// let version = Version::try_from("HTTP/1");
    /// assert!(version.is_ok());
    ///
    /// if let Some(version) = version {
    ///     assert_eq!(version.to_string(), "HTTP/1");
    /// }
    /// ```
    ///
    /// ```rust
    /// let version = Version::try_from("1.0");
    /// assert!(version.is_err());
    ///
    /// if let Err(error) = version {
    ///     assert_eq!(error, "Invalid HTTP version: '1.0'");
    /// }
    /// ```
    fn try_from(value: &str) -> Result<Version, Self::Error> {
        Self::try_from_line(value)
    }
}

impl TryFrom<String> for Version {
    type Error = InvalidHTTPVersionError;

    /// Get a constant reference to an HTTP version.
    ///
    /// # Parameters
    ///
    /// - `line`: The line must be in the form: `HTTP/{version}`.
    /// `{version}` can be like `major` or `major.minor`.
    ///
    /// # Returns
    ///
    /// Returns the constant to the good [`Version`], or a
    /// [`InvalidHTTPVersionError`] if `line` does not respect the pattern.
    ///
    /// # Examples
    ///
    /// ```rust
    /// let version = Version::try_from("HTTP/1");
    /// assert!(version.is_ok());
    ///
    /// if let Some(version) = version {
    ///     assert_eq!(version.to_string(), "HTTP/1");
    /// }
    /// ```
    ///
    /// ```rust
    /// let version = Version::try_from("1.0");
    /// assert!(version.is_err());
    ///
    /// if let Err(error) = version {
    ///     assert_eq!(error, "Invalid HTTP version: '1.0'");
    /// }
    /// ```
    fn try_from(value: String) -> Result<Version, Self::Error> {
        Self::try_from_line(value)
    }
}

impl TryFrom<&String> for Version {
    type Error = InvalidHTTPVersionError;

    /// Get a constant reference to an HTTP version.
    ///
    /// # Parameters
    ///
    /// - `line`: The line must be in the form: `HTTP/{version}`.
    /// `{version}` can be like `major` or `major.minor`.
    ///
    /// # Returns
    ///
    /// Returns the constant to the good [`Version`], or a
    /// [`InvalidHTTPVersionError`] if `line` does not respect the pattern.
    ///
    /// # Examples
    ///
    /// ```rust
    /// let version = Version::try_from("HTTP/1");
    /// assert!(version.is_ok());
    ///
    /// if let Some(version) = version {
    ///     assert_eq!(version.to_string(), "HTTP/1");
    /// }
    /// ```
    ///
    /// ```rust
    /// let version = Version::try_from("1.0");
    /// assert!(version.is_err());
    ///
    /// if let Err(error) = version {
    ///     assert_eq!(error, "Invalid HTTP version: '1.0'");
    /// }
    /// ```
    fn try_from(value: &String) -> Result<Version, Self::Error> {
        Self::try_from_line(value)
    }
}

/// Indicate that [`Version::try_from()`] reads an invalid HTTP version.
#[derive(Debug, Clone)]
pub struct InvalidHTTPVersionError {
    #[doc(hidden)]
    entry: String,
}

impl Display for InvalidHTTPVersionError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid HTTP version: '{}'", &self.entry)
    }
}

impl FromStr for InvalidHTTPVersionError {
    type Err = ();

    /// Create a new instance of [`InvalidHTTPVersionError`] with the invalid entry.
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`InvalidHTTPVersionError`].
    fn from_str(s: &str) -> Result<InvalidHTTPVersionError, ()> {
        Ok(Self::from(s))
    }
}

impl From<&str> for InvalidHTTPVersionError {
    /// Create a new instance of [`InvalidHTTPVersionError`] with the invalid entry.
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`InvalidHTTPVersionError`].
    fn from(value: &str) -> InvalidHTTPVersionError {
        Self {
            entry: value.to_string(),
        }
    }
}
