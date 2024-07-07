//! Module giving the implementation of [`WorkerPool`].
//!
//! To use [`WorkerPool`], go to the documentation of this class.

pub use self::pool::WorkerPool;

/// Module contains the [`WorkerPool`].
///
/// To use [`WorkerPool`], go to the documentation of this class.
mod pool;

/// Module contains the implementation details about [`Worker`](worker::Worker).
mod worker;
