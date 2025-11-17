#pragma once

#include <memory>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>

namespace tinker::tcp::connection {

using asio::ip::tcp;

class DaytimeConnection
    : public std::enable_shared_from_this<DaytimeConnection> {
  public:
    using pointer = std::shared_ptr<DaytimeConnection>;

    static auto create(asio::io_context& ctx) -> pointer {
        return pointer{new DaytimeConnection{ctx}};
    }

    auto socket() -> tcp::socket& { return socket_; }

    void start() {
        message_ = make_daytime_string();
        asio::async_write(
            socket_, asio::buffer(message_),
            [](const asio::error_code& err, std::size_t bytes_transferred) {});
    }

  private:
    static std::string make_daytime_string(
        const std::chrono::time_point<std::chrono::utc_clock>& timepoint =
            std::chrono::utc_clock::now()) {
        static constexpr auto format =
            "{0:%A}, {0:%B} {0:%d}, {0:%Y}, {0:%R}:{0:%S}-{0:%Z}";

        // Coarser duration for pretty printing
        auto tp = std::chrono::time_point_cast<std::chrono::seconds>(timepoint);
        return std::format(format, tp);
    }

    DaytimeConnection(asio::io_context& ctx) : socket_{ctx} {}

    tcp::socket socket_;
    std::string message_;
};

} // namespace tinker::tcp::connection