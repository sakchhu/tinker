#pragma once

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

namespace tinker::tcp {

using asio::ip::tcp;

// a very naive constraint for a tcp connection
template <class T>
concept connection_type = requires(T c, asio::io_context& ctx) {
    { c.socket() } -> std::convertible_to<tcp::socket&>;
    { c.start() };
    { T::create(ctx) } -> std::same_as<std::shared_ptr<T>>;
};

// Simple tcp server abstraction, expects a connection object that can be
// started and exposes it's socket handle to keep asynchronously accepting new
// connections until program is stopped
class AsyncServer {
  public:
    AsyncServer(asio::io_context& context, asio::ip::port_type port)
        : io_context{context},
          acceptor{io_context, tcp::endpoint{tcp::v4(), port}} {}

    template <connection_type ConnnectionT> void serve() {
        auto conn = ConnnectionT::create(io_context);
        acceptor.async_accept(conn->socket(),
                              [this, conn](const std::error_code& err) {
                                  if (!err) {
                                      conn->start();
                                  }

                                  this->serve<ConnnectionT>();
                              });
    }

  private:
    asio::io_context& io_context;
    asio::ip::tcp::acceptor acceptor;
};

} // namespace tinker::tcp