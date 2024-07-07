use std::fmt::{Debug, Display, Formatter};

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct Method {
    uri: String,
    method: String,
}

impl Display for Method {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", Self::convert(&self.method, &self.uri))
    }
}

impl Method {
    pub fn get(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::build("GET", uri)
    }
    pub fn post(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::build("POST", uri)
    }
    pub fn update(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::build("UPDATE", uri)
    }
    pub fn patch(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::build("PATCH", uri)
    }
    pub fn delete(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::build("DELETE", uri)
    }
    pub fn head(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::build("HEAD", uri)
    }
    pub fn options(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::build("OPTIONS", uri)
    }
    pub fn connect(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::build("CONNECT", uri)
    }
    pub fn trace(uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
        Self::build("TRACE", uri)
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
    pub fn from_line(line: impl AsRef<str>) -> Result<Self, Box<dyn InvalidMethodPartError>> {
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

    pub fn convert(method: impl AsRef<str>, uri: impl AsRef<str>) -> String {
        method.as_ref().to_uppercase() + "_" + uri.as_ref()
    }

    const ALLOWED_METHODS: &'static [&'static str] = &[
        "GET", "UPDATE", "POST", "DELETE", "PATCH", "HEAD", "OPTIONS", "CONNECT", "TRACE",
    ];
    fn build(method: impl AsRef<str>, uri: impl AsRef<str>) -> Result<Self, InvalidURIError> {
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

    fn check_uri(uri: impl AsRef<str>) -> Result<(), InvalidURIError> {
        let uri = uri.as_ref();
        if uri.trim().is_empty() {
            return Err(InvalidURIError::new(uri));
        }

        Ok(())
    }

    fn check_method(method: impl AsRef<str>) -> Result<(), InvalidMethodError> {
        let method = method.as_ref();
        if !Self::ALLOWED_METHODS.contains(&method) {
            return Err(InvalidMethodError::new(method));
        }

        Ok(())
    }
}

pub trait InvalidMethodPartError: Debug {}

#[derive(Debug, Clone)]
pub struct InvalidURIError {
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
pub struct InvalidMethodError {
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
