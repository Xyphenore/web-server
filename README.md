Multithreaded Web server
========================

Implementation of a multithreaded Web server like the
[exercise of the Rust Book](https://doc.rust-lang.org/book/ch20-00-final-project-a-web-server.html) but in CPP.

**Please don't use it or use it at your risks.**
You can use [Crow](https://github.com/ipkn/crow) or
[BoostBeast](https://www.boost.org/doc/libs/1_66_0/libs/beast/doc/html/beast/introduction.html).

[![Version](https://img.shields.io/badge/Version-v0.2.0-blue.svg)]()
[![C++](https://img.shields.io/badge/Language-C++17-blue.svg)]()
[![LGPL3.0 License](https://img.shields.io/badge/License-LGPL%20v3.0-green.svg)](https://www.gnu.org/licenses/lgpl-3.0.html)
[![Build](https://img.shields.io/badge/Build-Meson%201.4.0%20-graen.svg)](https://mesonbuild.com/index.html)

## Table of contents

* [Technologies](#tech-stack)
* [Setup](#setup)

## Tech Stack

Requirements:

- C++17
- Meson 1.4

## Setup

You can download the project and build it with Meson.

### Clone the project:

```bash
git clone https://github.com/Xyphenore/web-server.git
cd web-server
git checkout cpp-impl
```

### Build it:

Please edit 'gcc' and 'g++' by specific C/C++ compilers.
Compilers need to compile C++17.

```bash
CC=gcc CXX=g++ meson setup buildDir
cd buildDir && meson compile
```

### Run it

```shell
web-server
```

or

```shell
web-server.exe
```
