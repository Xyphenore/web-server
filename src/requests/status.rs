use std::cmp::Ordering;
use std::fmt::{Debug, Display, Formatter};

#[derive(Debug, Clone)]
pub struct Status {
    code: u16,
    name: &'static str,
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
}

impl Display for Status {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "{} {}", self.code, self.name)
    }
}

impl Status {
    pub const OK: &'static Self = &Self {
        code: 200,
        name: "OK",
    };
    pub const NOT_FOUND: &'static Self = &Self {
        code: 404,
        name: "NOT FOUND",
    };
}
