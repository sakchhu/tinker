// Simple Synchronous TCP daytime server
// Listens on port 8000, and sends daytime string in the form:
// Monday, November 17, 2025, 05:16:25-UTC

#include <asio.hpp>
#include <cx/logger.hpp>

#include <chrono>
#include <format>
#include <iostream>
#include <string>

std::string makeDaytimeString(
    const std::chrono::time_point<std::chrono::utc_clock>& timepoint =
        std::chrono::utc_clock::now()) {
    static constexpr auto format =
        "{0:%A}, {0:%B} {0:%d}, {0:%Y}, {0:%R}:{0:%S}-{0:%Z}";

    // Coarser duration for pretty printing
    auto tp = std::chrono::time_point_cast<std::chrono::seconds>(timepoint);
    return std::format(format, tp);
}

void logConnectionInfo(cx::Logger& logger,
                       const asio::ip::tcp::socket& socket) {
    auto endpoint = socket.remote_endpoint();
    logger.info("Connection accepted from: {}:{}",
                endpoint.address().to_string(), endpoint.port());
}

void serve(cx::Logger& logger, const asio::ip::tcp::endpoint& endpoint) {
    using asio::ip::tcp;

    asio::io_context ioCtx;
    tcp::acceptor acceptor{ioCtx, endpoint};

    logger.info("Accepting TCP/IPv4 connections at: {}:{}",
                endpoint.address().to_string(), endpoint.port());

    asio::error_code err;
    while (true) {
        tcp::socket socket{ioCtx};
        acceptor.accept(socket);

        logConnectionInfo(logger, socket);

        std::string message = makeDaytimeString();
        asio::write(socket, asio::buffer(message), err);

        if (err) {
            logger.warn("{}: {}", err.value(), err.message());
            continue;
        }
    }
}

int main() {
    auto logger = cx::Logger{std::cout};
    logger.prefix = "tds";
    logger.verbosity = cx::LogMsgVerbosity::Minimum;

    constexpr auto port = 8000;

    try {
        serve(logger, asio::ip::tcp::endpoint{
                          asio::ip::tcp::v4(),
                          port,
                      });
    } catch (const std::exception& err) {
        logger.error("{}", err.what());
    }

    return 0;
}