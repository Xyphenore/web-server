use std::fmt::{Debug, Display, Formatter};
use std::io::BufRead;

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct Method {
    pub uri: String,
    pub method: String,
}

impl Display for Method {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", Self::convert(&self.method, &self.uri))
    }
}

impl Method {
    pub fn get(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::new(uri, "GET", Panic::True)
    }
    pub fn post(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::new(uri, "POST", Panic::True)
    }
    pub fn update(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::new(uri, "UPDATE", Panic::True)
    }
    pub fn patch(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::new(uri, "PATCH", Panic::True)
    }
    pub fn delete(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::new(uri, "DELETE", Panic::True)
    }
    pub fn head(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::new(uri, "HEAD", Panic::True)
    }
    pub fn options(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::new(uri, "OPTIONS", Panic::True)
    }
    pub fn connect(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::new(uri, "CONNECT", Panic::True)
    }
    pub fn trace(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::new(uri, "TRACE", Panic::True)
    }

    /// Create a new instance from the line
    ///
    /// # Examples
    ///
    /// ```
    /// let method = Method::from_line("GET /");
    /// assert!(method.is_ok());
    /// if let Ok(method) = method {
    ///     assert_eq!(method.to_string(), "GET /");
    /// }
    /// ```
    ///
    /// ```
    /// let method = Method::from_line("NOT /");
    /// assert!(method.is_err());
    /// ```
    pub fn from_line(line: impl AsRef<str>) -> Result<Self, dyn InvalidMethodPartError> {
        let parts = line.as_ref().split(" ").collect();
        Self::new(parts[0], parts[1], Panic::False)
    }

    const ALLOWED_METHODS: Vec<&'static str> = vec![
        "GET", "UPDATE", "POST", "DELETE", "PATCH", "HEAD", "OPTIONS", "CONNECT", "TRACE",
    ];
    fn new(
        uri: impl AsRef<str>,
        method: impl AsRef<str>,
        panic: Panic,
    ) -> Result<Self, dyn InvalidMethodPartError> {
        Self::check_method(method.as_ref())
            .or_else(|error| {
                if panic == Panic::True {
                    panic!("The method {} is not allowed.", method);
                }

                Err(error)
            })
            .map_err(|error| -> dyn InvalidMethodPartError { Err(error) })
            .and(Self::check_uri(uri.as_ref()))
            .and(Ok(Self {
                method: method.as_ref().to_uppercase(),
                uri: uri.as_ref().to_string(),
            }))
    }

    pub fn convert(method: impl AsRef<str>, uri: impl AsRef<str>) -> String {
        method.as_ref().to_uppercase() + "_" + uri
    }

    fn check_uri(uri: impl AsRef<str>) -> Result<(), InvalidURIError> {
        if uri.as_ref().trim().is_empty() {
            return Err(InvalidURIError::new(uri));
        }

        Ok(())
    }

    fn check_method(method: impl AsRef<str>) -> Result<(), InvalidMethodError> {
        if !Self::ALLOWED_METHODS.contains(&method) {
            return Err(InvalidMethodError::new(method));
        }

        Ok(())
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
enum Panic {
    True,
    False,
}

trait InvalidMethodPartError: Debug + Clone {}

#[derive(Debug, Clone)]
struct InvalidURIError {
    entry: String,
}

impl Display for InvalidURIError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid URI: '{}'", &self.entry)
    }
}

impl InvalidURIError {
    fn new(entry: impl AsRef<str>) -> Self {
        Self {
            entry: entry.as_ref().to_string(),
        }
    }
}

impl InvalidMethodPartError for InvalidURIError {}

#[derive(Debug, Clone)]
struct InvalidMethodError {
    entry: String,
}

impl Display for InvalidMethodError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid Method: '{}'", &self.entry)
    }
}

impl InvalidMethodError {
    fn new(entry: impl AsRef<str>) -> Self {
        Self {
            entry: entry.as_ref().to_string(),
        }
    }
}

impl InvalidMethodPartError for InvalidMethodError {}
