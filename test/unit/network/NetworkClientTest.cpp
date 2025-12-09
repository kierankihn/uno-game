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
    auto on_connect = []() {};
    auto callback   = [](std::string message) {};

    EXPECT_NO_THROW({ NetworkClient client(on_connect, callback); });
}

TEST(NetworkClientTest, ConnectToHost)
{
    bool is_connected = false;

    SimpleTestServer server(30001);

    auto on_connect = [&is_connected]() { is_connected = true; };
    auto callback   = [](std::string message) {};
    NetworkClient client(on_connect, callback);

    auto netThread = std::thread([&client]() { client.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    client.connect("127.0.0.1", 30001);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    EXPECT_TRUE(is_connected);

    client.stop();
    if (netThread.joinable()) {
        netThread.join();
    }
}

TEST(NetworkClientTest, SendAfterConnect)
{
    SimpleTestServer server(30002);

    auto on_connect = []() {};
    auto callback   = [](std::string message) {};
    NetworkClient client(on_connect, callback);

    auto netThread = std::thread([&client]() { client.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    client.connect("127.0.0.1", 30002);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    client.send("Hello, Server!");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    client.stop();
    if (netThread.joinable()) {
        netThread.join();
    }
}
