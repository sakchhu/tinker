#include "tinker/connection/tcp/echo.hpp"
#include "tinker/server/async_tcp.hpp"

#include <asio/io_context.hpp>
#include <asio/write.hpp>
#include <exception>
#include <iostream>
#include <print>

#include <cx/logger.hpp>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::println("usage: async_echo <port>");
        return 1;
    }

    const auto port = argv[1];

    auto logger = cx::Logger{std::cout};
    logger.prefix = argv[0];

    try {
        auto context = asio::io_context{};
        auto server = tinker::tcp::AsyncServer{
            context, static_cast<asio::ip::port_type>(std::atoi(port))};
        server.serve<tinker::tcp::connection::Echo>();

        logger.info("Echo server started on port {}", port);

        context.run();
    } catch (const std::exception& err) {
        logger.error("Error: {}", err.what());
    }
}