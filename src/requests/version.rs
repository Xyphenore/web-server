use std::fmt::{Display, Formatter};

#[derive(Debug, Clone, PartialEq, PartialOrd, Eq, Ord, Hash)]
pub struct Version {
    #[doc(hidden)]
    major: char,
    #[doc(hidden)]
    minor: char,
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

impl Version {
    pub const HTTP_1: &'static Version = &Self {
        major: '1',
        minor: '0',
    };
    pub const HTTP_1_1: &'static Version = &Self {
        major: '1',
        minor: '1',
    };
    pub const HTTP_2: &'static Version = &Self {
        major: '2',
        minor: '0',
    };
    pub const HTTP_3: &'static Version = &Self {
        major: '3',
        minor: '0',
    };

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
            .copied()
            .ok_or(InvalidHTTPVersionError::new(line))
    }

    /// All allowed versions.
    #[doc(hidden)]
    const ALLOWED_VERSIONS: &'static [&'static Self] =
        &[Self::HTTP_1, Self::HTTP_1_1, Self::HTTP_2, Self::HTTP_3];
}

#[derive(Debug, Clone)]
pub struct InvalidHTTPVersionError {
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
