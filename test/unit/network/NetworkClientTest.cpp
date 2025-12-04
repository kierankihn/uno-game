/**
 * @file NetworkClientTest.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.28
 */

#include "../../../src/network/NetworkClient.h"

#include <gtest/gtest.h>

using namespace UNO::NETWORK;

class SimpleTestServer {
private:
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::thread server_thread_;
    std::atomic<bool> running_{true};

public:
    SimpleTestServer(uint16_t port) : acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    {
        server_thread_ = std::thread([this]() { runServer(); });
    }

    ~SimpleTestServer()
    {
        running_ = false;
        asio::error_code ec;
        acceptor_.close(ec);
        io_context_.stop();
        if (server_thread_.joinable()) {
            server_thread_.join();
        }
    }

private:
    void runServer()
    {
        while (running_) {
            try {
                asio::ip::tcp::socket socket(io_context_);
                acceptor_.accept(socket);

                // Echo server: read message and send it back
                while (socket.is_open() && running_) {
                    try {
                        size_t length;
                        // Use async read with timeout simulation via non-blocking approach
                        asio::error_code ec;

                        // Try to read the length with error handling
                        asio::read(socket, asio::buffer(&length, sizeof(length)), ec);
                        if (ec) {
                            break;   // Connection closed or error
                        }

                        if (length <= 10 * 1024 * 1024) {
                            std::vector<char> buffer(length);
                            asio::read(socket, asio::buffer(buffer), ec);
                            if (ec) {
                                break;   // Connection closed or error
                            }

                            // Echo back
                            asio::write(socket, asio::buffer(&length, sizeof(length)), ec);
                            if (ec) {
                                break;
                            }
                            asio::write(socket, asio::buffer(buffer), ec);
                            if (ec) {
                                break;
                            }
                        }
                    }
                    catch (...) {
                        break;
                    }
                }

                // Ensure socket is closed when exiting the loop
                asio::error_code ec;
                socket.close(ec);
            }
            catch (...) {
                if (!running_) {
                    break;
                }
            }
        }
    }
};

TEST(NetworkClientTest, ConstructorWithCallback)
{
    auto callback = [](std::string message) {};

    EXPECT_NO_THROW({ NetworkClient client(callback); });
}

TEST(NetworkClientTest, ConnectToHost)
{
    SimpleTestServer server(30001);

    auto callback = [](std::string message) {};
    NetworkClient client(callback);

    EXPECT_NO_THROW({ client.connect("localhost", 30001); });
}

TEST(NetworkClientTest, ConnectToInvalidHost)
{
    SimpleTestServer server(30002);

    auto callback = [](std::string message) {};
    NetworkClient client(callback);

    EXPECT_THROW({ client.connect("invalid.host.that.does.not.exist.12345", 30002); }, std::exception);
}

TEST(NetworkClientTest, SendAfterConnect)
{
    SimpleTestServer server(30003);

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("localhost", 30003);

    EXPECT_NO_THROW({ client.send("Hello, Server!"); });
}
