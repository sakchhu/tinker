// Asynchronous daytime server

#include <asio/io_context.hpp>

#include <exception>
#include <print>

#include "tinker/connection/tcp/daytime.hpp"
#include "tinker/server/async_tcp.hpp"

int main() {
    try {
        constexpr auto port = 8000;
        auto ctx = asio::io_context{};
        auto server = tinker::tcp::AsyncServer{ctx, port};
        server.serve<tinker::tcp::connection::DaytimeConnection>();

        ctx.run();
    } catch (const std::exception& err) {
        std::println("{}", err.what());
    }
}