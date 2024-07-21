Multithreaded Web server
========================

Implementation of a multithreaded Web server like the
[exercise of the Rust Book](https://doc.rust-lang.org/book/ch20-00-final-project-a-web-server.html).

**Please don't use it or use it at your risks.**
You can use [ActixWeb](https://actix.rs/).

[![Version](https://img.shields.io/badge/Version-v0.11.0-blue.svg)]()
[![Rust](https://img.shields.io/badge/Language-Rust2021-blue.svg)]()
[![LGPL3.0 License](https://img.shields.io/badge/License-LGPL%20v3.0-green.svg)](https://www.gnu.org/licenses/lgpl-3.0.html)
![Build](https://img.shields.io/badge/Build-Cargo%201.79.0%20-graen.svg)

## Table of contents

* [Technologies](#tech-stack)
* [Setup](#setup)

## Tech Stack

Requirements:

- Rust 2021
- Cargo 1.79.0

## Setup

You can download the project and build it with Meson.

### Clone the project:

```bash
git clone https://github.com/Xyphenore/web-server.git
```

### Build it:

```shell
cargo build
```

### Run it

```shell
cargo run
```

### Generate the documentation

```shell
cargo doc
```

Open the [target/doc/web_server/index.html](target/doc/web_server/index.html) in the directory
[target/doc/web_server](target/doc/web_server).

```shell
cd target/doc/web_server
```
