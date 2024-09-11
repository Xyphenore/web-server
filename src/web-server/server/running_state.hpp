#ifndef SERVER_RUNNING_STATE_HPP
#define SERVER_RUNNING_STATE_HPP

#include "io_context.hpp"

#include <atomic>

namespace web_server::server {
    class [[nodiscard]] RunningState final {
        public:
            RunningState() noexcept = default;

            explicit RunningState(IOContext& io_context) noexcept;

            RunningState& operator=(bool state) noexcept;

            [[nodiscard]] explicit operator bool() const noexcept;

        private:
            [[nodiscard]] friend bool operator==(const RunningState& lhs, const RunningState& rhs) noexcept {
                return lhs.is_running_ == rhs.is_running_;
            }

            [[nodiscard]] friend bool operator!=(const RunningState& lhs, const RunningState& rhs) noexcept {
                return not(lhs == rhs);
            }

            std::atomic_bool is_running_{false};
    };
} // namespace web_server::server

namespace web_server::server {
    inline RunningState& RunningState::operator=(const bool state) noexcept {
        is_running_ = state;
        return *this;
    }

    inline RunningState::operator bool() const noexcept { return is_running_; }
} // namespace web_server::server

#endif // SERVER_RUNNING_STATE_HPP
