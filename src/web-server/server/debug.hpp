#ifndef SERVER_DEBUG_HPP
#define SERVER_DEBUG_HPP

namespace web_server::server {
    /// Indicate if the debug mode is activated on the [`WebServer`].
    enum class [[nodiscard]] Debug : bool {
        On = true,
        Off = false,
    };
} // namespace web_server::server

#endif // SERVER_DEBUG_HPP
