#pragma once

#include <array>
#include <memory>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>

namespace tinker::tcp::connection {

using asio::ip::tcp;

class Echo : public std::enable_shared_from_this<Echo> {
  public:
    using pointer = std::shared_ptr<Echo>;

    static auto create(asio::io_context& context) {
        return pointer(new Echo{context});
    }

    auto socket() -> tcp::socket& { return socket_; }
    auto start() { read_message(); }

  private:
    Echo(asio::io_context& context) : socket_{context} {}

    void read_message() {
        auto keepalive{shared_from_this()}; // ensure the connection is alive
                                            // after finished reading
        socket_.async_read_some(asio::buffer(message_, max_length),
                                [this, keepalive](const asio::error_code& errc,
                                                  std::size_t bytes_read) {
                                    if (!errc) {
                                        write_message(bytes_read);
                                    }
                                });
    }

    void write_message(std::size_t length) {
        auto keepalive{shared_from_this()};
        asio::async_write(socket_, asio::buffer(message_, length),
                          [this, keepalive](const std::error_code& errc,
                                            std::size_t bytes_written) {
                              if (!errc) {
                                  read_message();
                              }
                          });
    }

    static constexpr auto max_length = 1024;
    std::array<char, max_length> message_;
    tcp::socket socket_;
};

} // namespace tinker::tcp::connection
