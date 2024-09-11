#ifndef HELPERS_COMPAT_HPP
#define HELPERS_COMPAT_HPP

#include "sockets.hpp"

#include <fmt/base.h>
#include <fmt/ostream.h>

template <>
struct fmt::formatter<web_server::helpers::RemoteAddress> final: public fmt::ostream_formatter {};

template <>
struct fmt::formatter<web_server::helpers::LocalEndPoint> final: public fmt::ostream_formatter {};

#endif // HELPERS_COMPAT_HPP
