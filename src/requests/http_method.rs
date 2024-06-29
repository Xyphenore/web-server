#[derive(Debug)]
pub enum HTTPMethod {
    GET(String),
    POST(String),
    UPDATE(String),
    PATCH(String),
    DELETE(String),
    OPTIONS(String),
}

impl HTTPMethod {
    pub fn method(&self) -> String {
        let binding = format!("{:?}", self);
        let parts: Vec<_> = binding.split("(").collect();
        parts[0].to_uppercase()
    }

    pub fn uri(&self) -> String {
        match self {
            HTTPMethod::GET(uri)
            | HTTPMethod::POST(uri)
            | HTTPMethod::UPDATE(uri)
            | HTTPMethod::PATCH(uri)
            | HTTPMethod::DELETE(uri)
            | HTTPMethod::OPTIONS(uri) => uri.to_string(),
        }
    }

    pub fn to_string(&self) -> String {
        HTTPMethod::convert(&self.method(), &self.uri())
    }

    pub fn convert(method: &str, uri: &str) -> String {
        method.to_uppercase() + "_" + uri
    }
}
