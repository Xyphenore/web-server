#ifndef HELPERS_THREADS_HPP
#define HELPERS_THREADS_HPP

#include <thread>
#include <type_traits>
#include <utility>

namespace web_server::helpers {
    class [[nodiscard]] Thread final {
            template <typename T>
            using RemovedCVRefType = std::remove_cv_t<std::remove_reference_t<T>>;

            template <typename T>
            using IsNotThread = std::enable_if_t<(not std::is_same_v<RemovedCVRefType<T>, std::thread>)and(
                not std::is_same_v<RemovedCVRefType<T>, Thread>)>;

        public:
            Thread() noexcept = delete;

            template <typename Callable, typename... Args, typename = IsNotThread<Callable>>
            explicit Thread(Callable&& function, Args&&... args):
            handle_{std::forward<Callable>(function), std::forward<Args>(args)...} {}

            Thread(const Thread&) noexcept = delete;
            Thread(Thread&& other) noexcept;

            Thread& operator=(const Thread&) noexcept = delete;
            Thread& operator=(Thread&& other) noexcept;

            ~Thread() noexcept;

        private:
            friend void swap(Thread& lhs, Thread& rhs) noexcept { std::swap(lhs.handle_, rhs.handle_); }

            [[nodiscard]] friend bool operator==(const Thread& lhs, const Thread& rhs) noexcept {
                return lhs.handle_.get_id() == rhs.handle_.get_id();
            }

            [[nodiscard]] friend bool operator!=(const Thread& lhs, const Thread& rhs) noexcept {
                return not(lhs == rhs);
            }

            std::thread handle_;
    };
} // namespace web_server::helpers

namespace web_server::helpers {
    inline Thread::~Thread() noexcept {
        if (handle_.joinable()) {
            handle_.join();
        }
    }

    inline Thread& Thread::operator=(Thread&& other) noexcept {
        if (&other != this) {
            if (handle_.joinable()) {
                handle_.join();
            }

            handle_ = std::move(other.handle_);
        }

        return *this;
    }

    inline Thread::Thread(Thread&& other) noexcept: handle_{std::move(other.handle_)} {}
} // namespace web_server::helpers

#endif // HELPERS_THREADS_HPP
