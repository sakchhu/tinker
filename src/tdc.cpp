#include <asio.hpp>
#include <cx/logger.hpp>
#include <iostream>
#include <print>

void print_response(asio::ip::tcp::socket& socket) {
    constexpr std::size_t bufSize = 128;

    std::array<char, bufSize> buffer;
    std::error_code errc;

    while (true) {
        auto len = socket.read_some(asio::buffer(buffer), errc);
        if (errc == asio::error::eof) {
            // all the response already transmitted
            break;
        } else if (errc) {
            throw std::system_error(errc);
        }

        // convert the filled char array to an actual string view so
        // std::print can print the string representation instead of the
        // array representation
        std::println("{}", std::string_view{buffer.data(), len});
    }
}

int main(int argc, char* argv[]) {
    auto logger = cx::Logger{std::cout};
    constexpr auto name = "tdc";
    logger.prefix = name;
    logger.minimumLevel = cx::LogLevel::Trace;
    logger.verbosity = cx::LogMsgVerbosity::Minimum;

    if (argc != 2) {
        std::println("usage: {} <host>", name);
        return 1;
    }

    const auto host = argv[1];       // hostname
    constexpr auto service = "8000"; // service name / port

    try {
        using asio::ip::tcp;
        auto ioCtx = asio::io_context{};

        // resolves both "hostname:service" and "address:port" formats
        auto resolver = tcp::resolver{ioCtx};

        auto endpoints = resolver.resolve(host, service);
        auto socket = tcp::socket{ioCtx};
        auto connected_to = asio::connect(socket, endpoints);
        logger.trace("Connected to {}:{}", connected_to.address().to_string(),
                     connected_to.port());

        print_response(socket);

    } catch (const std::exception& err) {
        logger.error("{}", err.what());
    }
}