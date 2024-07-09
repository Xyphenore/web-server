use std::error::Error;
use std::fmt::{Debug, Display, Formatter};
use std::str::FromStr;

/// HTTP method
///
/// [MDN - Methods](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods)
///
/// # How to use it?
///
/// # Class method
///
/// ```rust
/// use crate::requests::Method;
///
/// let get_index = Method::get("/");
/// let post_index = Method::post("/");
/// let update_index = Method::update("/");
/// let delete_index = Method::delete("/");
/// let options_index = Method::options("/");
/// let head_index = Method::head("/");
/// let patch_index = Method::patch("/");
/// let trace_index = Method::trace("/");
/// let connect_index = Method::connect("/");
/// ```
///
/// # From a string
///
/// ```
/// use crate::requests::Method;
///
/// let method = Method::try_from("GET /");
/// assert!(method.is_ok());
/// if let Ok(method) = method {
///     assert_eq!(method.to_string(), "GET /");
/// }
/// ```
///
/// ```
/// use crate::requests::Method;
///
/// let method = Method::try_from("NOT /");
/// assert!(method.is_err());
/// ```
///
/// ```
/// use crate::requests::Method;
///
/// let method = Method::from_str("GET /");
/// assert!(method.is_ok());
/// if let Ok(method) = method {
///     assert_eq!(method.to_string(), "GET /");
/// }
/// ```
///
/// ```
/// use crate::requests::Method;
///
/// let method = Method::from_str("NOT /");
/// assert!(method.is_err());
/// ```
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct Method {
    #[doc(hidden)]
    verb: &'static str,
    #[doc(hidden)]
    uri: String,
}

impl Method {
    /// Create a `GET` method with the URI.
    ///
    /// # Parameters
    ///
    /// - `uri`: The URI must be not blank.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let get_index = Method::get("/");
    /// ```
    pub fn get(uri: impl AsRef<str>) -> Result<Method, InvalidMethodError> {
        Self::build("GET", uri)
    }

    /// Create a `POST` method with the URI.
    ///
    /// # Parameters
    ///
    /// - `uri`: The URI must be not blank.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let post_index = Method::post("/");
    /// ```
    pub fn post(uri: impl AsRef<str>) -> Result<Method, InvalidMethodError> {
        Self::build("POST", uri)
    }

    /// Create an `UPDATE` method with the URI.
    ///
    /// # Parameters
    ///
    /// - `uri`: The URI must be not blank.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let update_index = Method::update("/");
    /// ```
    pub fn update(uri: impl AsRef<str>) -> Result<Method, InvalidMethodError> {
        Self::build("UPDATE", uri)
    }

    /// Create a `PATCH` method with the URI.
    ///
    /// # Parameters
    ///
    /// - `uri`: The URI must be not blank.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let patch_index = Method::patch("/");
    /// ```
    pub fn patch(uri: impl AsRef<str>) -> Result<Method, InvalidMethodError> {
        Self::build("PATCH", uri)
    }

    /// Create a `DELETE` method with the URI.
    ///
    /// # Parameters
    ///
    /// - `uri`: The URI must be not blank.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let delete_index = Method::delete("/");
    /// ```
    pub fn delete(uri: impl AsRef<str>) -> Result<Method, InvalidMethodError> {
        Self::build("DELETE", uri)
    }

    /// Create a `HEAD` method with the URI.
    ///
    /// # Parameters
    ///
    /// - `uri`: The URI must be not blank.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let head_index = Method::head("/");
    /// ```
    pub fn head(uri: impl AsRef<str>) -> Result<Method, InvalidMethodError> {
        Self::build("HEAD", uri)
    }

    /// Create an `OPTIONS` method with the URI.
    ///
    /// # Parameters
    ///
    /// - `uri`: The URI must be not blank.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let options_index = Method::options("/");
    /// ```
    pub fn options(uri: impl AsRef<str>) -> Result<Method, InvalidMethodError> {
        Self::build("OPTIONS", uri)
    }

    /// Create a `CONNECT` method with the URI.
    ///
    /// # Parameters
    ///
    /// - `uri`: The URI must be not blank.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let connect_index = Method::connect("/");
    /// ```
    pub fn connect(uri: impl AsRef<str>) -> Result<Method, InvalidMethodError> {
        Self::build("CONNECT", uri)
    }

    /// Create a `TRACE` method with the URI.
    ///
    /// # Parameters
    ///
    /// - `uri`: The URI must be not blank.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns the new instance [`Method`], or [`InvalidMethodError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let trace_index = Method::trace("/");
    /// ```
    pub fn trace(uri: impl AsRef<str>) -> Result<Method, InvalidMethodError> {
        Self::build("TRACE", uri)
    }

    /// Create a new instance of [`Method`] from the line.
    ///
    /// # Parameters
    ///
    /// - `line`: The line must start like: `METHOD URI`.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns the instance of [`Method`], or the error if `line` is invalid.
    #[doc(hidden)]
    fn try_from_line(line: impl AsRef<str>) -> Result<Method, InvalidMethodError> {
        let parts: Vec<_> = line.as_ref().split(' ').take(2).collect();

        let upper_verb = parts[0].to_uppercase();
        let uri = parts[1];

        Self::check_uri(uri)
            .and(
                Self::ALLOWED_METHODS
                    .iter()
                    .find(|verb| **verb == upper_verb)
                    .ok_or_else(|| InvalidMethodError::InvalidVerbError(parts[0].to_owned())),
            )
            .map(|verb| Self {
                verb,
                uri: uri.to_owned(),
            })
    }

    /// All allowed methods in the HTTP protocol.
    ///
    /// [MDN - Methods](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods)
    #[doc(hidden)]
    const ALLOWED_METHODS: &'static [&'static str] = &[
        "GET", "UPDATE", "POST", "DELETE", "PATCH", "HEAD", "OPTIONS", "CONNECT", "TRACE",
    ];

    /// Build a [`Method`] with the `method` and `uri`.
    ///
    /// # Parameters
    ///
    /// - `method`: A value in [`Method::ALLOWED_METHODS`].
    /// - `uri`: The URI of the method.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`Method`] if all is good or an [`InvalidMethodError`]
    /// if the `uri` is blank.
    ///
    /// # Panics
    ///
    /// If `method` is not in [`Method::ALLOWED_METHODS`].
    #[doc(hidden)]
    fn build(method: &'static str, uri: impl AsRef<str>) -> Result<Method, InvalidMethodError> {
        let uri = uri.as_ref();

        if !Self::ALLOWED_METHODS.contains(&method) {
            unreachable!("The method {} is not allowed.", method);
        }

        Self::check_uri(uri).and(Ok(Self {
            verb: method,
            uri: uri.to_string(),
        }))
    }

    /// Check if `uri` is not empty or blank.
    ///
    /// # Parameters
    ///
    /// - `uri`: The URI of the method.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns nothing if the `uri` is good, or [`InvalidMethodError`] if the `uri` is
    /// blank.
    #[doc(hidden)]
    fn check_uri(uri: impl AsRef<str>) -> Result<(), InvalidMethodError> {
        let uri = uri.as_ref();
        if uri.trim().is_empty() {
            return Err(InvalidMethodError::InvalidURIError(uri.to_owned()));
        }

        Ok(())
    }
}

impl Display for Method {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}_{}", self.verb.to_uppercase(), &self.uri)
    }
}

impl FromStr for Method {
    type Err = InvalidMethodError;

    /// Create a new instance of [`Method`] from the line.
    ///
    /// # Parameters
    ///
    /// - `s`: The line must start like: `METHOD URI`.
    ///
    /// # Returns
    ///
    /// Returns the instance of [`Method`], or the error if `line` is invalid.
    fn from_str(s: &str) -> Result<Method, Self::Err> {
        Self::try_from(s)
    }
}

impl TryFrom<&str> for Method {
    type Error = InvalidMethodError;

    /// Create a new instance of [`Method`] from the line.
    ///
    /// # Parameters
    ///
    /// - `value`: The line must start like: `METHOD URI`.
    ///
    /// # Returns
    ///
    /// Returns the instance of [`Method`], or the error if `line` is invalid.
    fn try_from(value: &str) -> Result<Method, Self::Error> {
        Self::try_from_line(value)
    }
}

impl TryFrom<String> for Method {
    type Error = InvalidMethodError;

    /// Create a new instance of [`Method`] from the line.
    ///
    /// # Parameters
    ///
    /// - `value`: The line must start like: `METHOD URI`.
    ///
    /// # Returns
    ///
    /// Returns the instance of [`Method`], or the error if `line` is invalid.
    fn try_from(value: String) -> Result<Method, Self::Error> {
        Self::try_from_line(value)
    }
}

impl TryFrom<&String> for Method {
    type Error = InvalidMethodError;

    /// Create a new instance of [`Method`] from the line.
    ///
    /// # Parameters
    ///
    /// - `value`: The line must start like: `METHOD URI`.
    ///
    /// # Returns
    ///
    /// Returns the instance of [`Method`], or the error if `line` is invalid.
    fn try_from(value: &String) -> Result<Method, Self::Error> {
        Self::try_from_line(value)
    }
}

/// Indicate that [`Method::try_from()`] reads an invalid part (URI, Method).
#[derive(Debug, Clone)]
pub enum InvalidMethodError {
    /// Indicate that [`Method::try_from()`] reads an invalid URI.
    InvalidURIError(String),
    /// Indicate that [`Method::try_from()`] reads an invalid method verb.
    InvalidVerbError(String),
}

impl Display for InvalidMethodError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let msg = match self {
            Self::InvalidURIError(entry) => format!("Invalid URI: '{}'", entry),
            Self::InvalidVerbError(entry) => format!("Invalid Method: '{}'", entry),
        };

        write!(f, "{}", msg)
    }
}

impl Error for InvalidMethodError {}
