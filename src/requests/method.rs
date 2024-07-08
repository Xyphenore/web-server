use std::fmt::{Debug, Display, Formatter};

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
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct Method {
    #[doc(hidden)]
    uri: String,
    #[doc(hidden)]
    method: String,
}

impl Display for Method {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}_{}", self.method.to_uppercase(), &self.uri)
    }
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
    /// Returns the new instance [`Method`], or [`InvalidURIError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let get_index = Method::get("/");
    /// ```
    pub fn get(uri: impl AsRef<str>) -> Result<Method, InvalidURIError> {
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
    /// Returns the new instance [`Method`], or [`InvalidURIError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let post_index = Method::post("/");
    /// ```
    pub fn post(uri: impl AsRef<str>) -> Result<Method, InvalidURIError> {
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
    /// Returns the new instance [`Method`], or [`InvalidURIError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let update_index = Method::update("/");
    /// ```
    pub fn update(uri: impl AsRef<str>) -> Result<Method, InvalidURIError> {
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
    /// Returns the new instance [`Method`], or [`InvalidURIError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let patch_index = Method::patch("/");
    /// ```
    pub fn patch(uri: impl AsRef<str>) -> Result<Method, InvalidURIError> {
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
    /// Returns the new instance [`Method`], or [`InvalidURIError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let delete_index = Method::delete("/");
    /// ```
    pub fn delete(uri: impl AsRef<str>) -> Result<Method, InvalidURIError> {
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
    /// Returns the new instance [`Method`], or [`InvalidURIError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let head_index = Method::head("/");
    /// ```
    pub fn head(uri: impl AsRef<str>) -> Result<Method, InvalidURIError> {
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
    /// Returns the new instance [`Method`], or [`InvalidURIError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let options_index = Method::options("/");
    /// ```
    pub fn options(uri: impl AsRef<str>) -> Result<Method, InvalidURIError> {
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
    /// Returns the new instance [`Method`], or [`InvalidURIError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let connect_index = Method::connect("/");
    /// ```
    pub fn connect(uri: impl AsRef<str>) -> Result<Method, InvalidURIError> {
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
    /// Returns the new instance [`Method`], or [`InvalidURIError`] if `uri` is blank.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use crate::requests::Method;
    ///
    /// let trace_index = Method::trace("/");
    /// ```
    pub fn trace(uri: impl AsRef<str>) -> Result<Method, InvalidURIError> {
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
    fn try_from_line(line: impl AsRef<str>) -> Result<Method, Box<dyn InvalidMethodPartError>> {
        let parts: Vec<_> = line.as_ref().split(' ').collect();
        let method = parts[0];
        let uri = parts[1];

        Self::check_method(method)
            .map_err(|error| -> Box<dyn InvalidMethodPartError> { Box::new(error) })
            .and(
                Self::check_uri(uri)
                    .map_err(|error| -> Box<dyn InvalidMethodPartError> { Box::new(error) }),
            )
            .and(Ok(Self {
                method: method.to_uppercase(),
                uri: uri.to_string(),
            }))
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
    /// A [`String`] or [`&str`].
    /// - `uri`: The URI of the method.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`Method`] if all is good or an [`InvalidURIError`]
    /// if the `uri` is blank.
    ///
    /// # Panics
    ///
    /// If `method` is not in [`Method::ALLOWED_METHODS`].
    #[doc(hidden)]
    fn build(method: impl AsRef<str>, uri: impl AsRef<str>) -> Result<Method, InvalidURIError> {
        let method = method.as_ref();
        let uri = uri.as_ref();

        Self::check_method(method)
            .map_err(|_| panic!("The method {} is not allowed.", method))
            .and(Self::check_uri(uri))
            .and(Ok(Self {
                method: method.to_uppercase(),
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
    /// Returns nothing if the `uri` is good, or [`InvalidURIError`] if the `uri` is
    /// blank.
    #[doc(hidden)]
    fn check_uri(uri: impl AsRef<str>) -> Result<(), InvalidURIError> {
        let uri = uri.as_ref();
        if uri.trim().is_empty() {
            return Err(InvalidURIError::from(uri));
        }

        Ok(())
    }

    /// Check if `method` is in [`Method::ALLOWED_METHODS`].
    ///
    /// # Parameters
    ///
    /// - `method`: The verb of the method.
    /// A [`String`] or [`&str`].
    ///
    /// # Returns
    ///
    /// Returns nothing if the `method` is in [`Method::ALLOWED_METHODS`], or
    /// [`InvalidMethodError`] if the `method` is blank.
    #[doc(hidden)]
    fn check_method(method: impl AsRef<str>) -> Result<(), InvalidMethodError> {
        let method = method.as_ref();
        if !Self::ALLOWED_METHODS.contains(&method) {
            return Err(InvalidMethodError::from(method));
        }

        Ok(())
    }
}

impl TryFrom<&str> for Method {
    type Error = Box<dyn InvalidMethodPartError>;

    /// Create a new instance of [`Method`] from the line.
    ///
    /// # Parameters
    ///
    /// - `line`: The line must start like: `METHOD URI`.
    ///
    /// # Returns
    ///
    /// Returns the instance of [`Method`], or the error if `line` is invalid.
    fn try_from(value: &str) -> Result<Method, Self::Error> {
        Self::try_from_line(value)
    }
}

impl TryFrom<String> for Method {
    type Error = Box<dyn InvalidMethodPartError>;

    /// Create a new instance of [`Method`] from the line.
    ///
    /// # Parameters
    ///
    /// - `line`: The line must start like: `METHOD URI`.
    ///
    /// # Returns
    ///
    /// Returns the instance of [`Method`], or the error if `line` is invalid.
    fn try_from(value: String) -> Result<Method, Self::Error> {
        Self::try_from_line(value)
    }
}

impl TryFrom<&String> for Method {
    type Error = Box<dyn InvalidMethodPartError>;

    /// Create a new instance of [`Method`] from the line.
    ///
    /// # Parameters
    ///
    /// - `line`: The line must start like: `METHOD URI`.
    ///
    /// # Returns
    ///
    /// Returns the instance of [`Method`], or the error if `line` is invalid.
    fn try_from(value: &String) -> Result<Method, Self::Error> {
        Self::try_from_line(value)
    }
}

/// Indicate that [`Method::try_from()`] reads an invalid part (URI, Method).
pub trait InvalidMethodPartError: Debug + Display {}

/// Indicate that [`Method::try_from()`] reads an invalid URI.
#[derive(Debug, Clone)]
pub struct InvalidURIError {
    #[doc(hidden)]
    entry: String,
}

impl Display for InvalidURIError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid URI: '{}'", &self.entry)
    }
}

impl From<&str> for InvalidURIError {
    /// Create a new instance of [`InvalidURIError`] with the invalid entry.
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`InvalidURIError`].
    fn from(value: &str) -> InvalidURIError {
        Self {
            entry: value.to_owned(),
        }
    }
}

impl InvalidMethodPartError for InvalidURIError {}

/// Indicate that [`Method::try_from()`] reads an invalid method verb.
#[derive(Debug, Clone)]
pub struct InvalidMethodError {
    #[doc(hidden)]
    entry: String,
}

impl Display for InvalidMethodError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid Method: '{}'", &self.entry)
    }
}

impl From<&str> for InvalidMethodError {
    /// Create a new instance of [`InvalidMethodError`] with the invalid entry.
    ///
    /// # Returns
    ///
    /// Returns a new instance of [`InvalidMethodError`].
    fn from(value: &str) -> InvalidMethodError {
        Self {
            entry: value.to_owned(),
        }
    }
}

impl InvalidMethodPartError for InvalidMethodError {}
