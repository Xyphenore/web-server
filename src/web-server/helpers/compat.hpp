#ifndef HELPERS_COMPAT_HPP
#define HELPERS_COMPAT_HPP

#include "sockets.hpp"

#include <fmt/base.h>
#include <fmt/ostream.h>

namespace fmt {
    template <>
    struct formatter<web_server::helpers::RemoteAddress> final: public ostream_formatter {};

    template <>
    struct formatter<web_server::helpers::LocalEndPoint> final: public ostream_formatter {};
} // namespace fmt

#endif // HELPERS_COMPAT_HPP
