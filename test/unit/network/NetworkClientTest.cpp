/**
 * @file NetworkClientTest.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.28
 */

#include "../../../src/network/NetworkClient.h"

#include <asio.hpp>
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

using namespace UNO::NETWORK;

// Helper class to create a simple echo server for testing
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

// ========== NetworkClient Constructor Tests ==========

TEST(NetworkClientTest, ConstructorWithCallback)
{
    auto callback = [](std::string message) {};

    EXPECT_NO_THROW({ NetworkClient client(callback); });
}

TEST(NetworkClientTest, ConstructorWithEmptyCallback)
{
    auto callback = [](std::string message) {};

    EXPECT_NO_THROW({ NetworkClient client(callback); });
}

// ========== NetworkClient Connect Tests ==========

TEST(NetworkClientTest, ConnectToValidServer)
{
    SimpleTestServer server(30001);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);

    EXPECT_NO_THROW({ client.connect("127.0.0.1", 30001); });
}

TEST(NetworkClientTest, ConnectToLocalhost)
{
    SimpleTestServer server(30002);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);

    EXPECT_NO_THROW({ client.connect("localhost", 30002); });
}

TEST(NetworkClientTest, ConnectToInvalidHost)
{
    auto callback = [](std::string message) {};
    NetworkClient client(callback);

    // Should throw when trying to resolve invalid host
    EXPECT_THROW({ client.connect("invalid.host.that.does.not.exist.12345", 30003); }, std::exception);
}

TEST(NetworkClientTest, ConnectToUnreachablePort)
{
    auto callback = [](std::string message) {};
    NetworkClient client(callback);

    // Should throw when trying to connect to a port with no server
    EXPECT_THROW({ client.connect("127.0.0.1", 30004); }, std::exception);
}

// ========== NetworkClient Send Tests ==========

TEST(NetworkClientTest, SendSimpleMessage)
{
    SimpleTestServer server(30005);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30005);

    EXPECT_NO_THROW({ client.send("Hello, Server!"); });
}

TEST(NetworkClientTest, SendEmptyMessage)
{
    SimpleTestServer server(30006);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30006);

    EXPECT_NO_THROW({ client.send(""); });
}

TEST(NetworkClientTest, SendLargeMessage)
{
    SimpleTestServer server(30007);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30007);

    std::string large_message(1024 * 1024, 'X');   // 1MB message
    EXPECT_NO_THROW({ client.send(large_message); });
}

TEST(NetworkClientTest, SendMessageWithSpecialCharacters)
{
    SimpleTestServer server(30008);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30008);

    EXPECT_NO_THROW({ client.send("Special: \n\t\r\"'\\"); });
}

TEST(NetworkClientTest, SendMessageWithUnicode)
{
    SimpleTestServer server(30009);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30009);

    EXPECT_NO_THROW({ client.send("Unicode: 你好世界 🎮🃏"); });
}

TEST(NetworkClientTest, SendMultipleMessages)
{
    SimpleTestServer server(30010);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30010);

    for (int i = 0; i < 10; ++i) {
        EXPECT_NO_THROW({ client.send("Message " + std::to_string(i)); });
    }
}

TEST(NetworkClientTest, SendJSONMessage)
{
    SimpleTestServer server(30011);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30011);

    std::string json_message = R"({"status":"OK","type":"TEST","data":"value"})";
    EXPECT_NO_THROW({ client.send(json_message); });
}

// ========== NetworkClient Read Tests ==========

TEST(NetworkClientTest, ReadMessageFromServer)
{
    SimpleTestServer server(30012);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30012);

    std::string test_message = "Test Message";
    client.send(test_message);

    std::string received;
    EXPECT_NO_THROW({ received = client.read(); });

    EXPECT_EQ(received, test_message);
}

TEST(NetworkClientTest, ReadEmptyMessage)
{
    SimpleTestServer server(30013);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30013);

    client.send("");

    std::string received;
    EXPECT_NO_THROW({ received = client.read(); });

    EXPECT_EQ(received, "");
}

TEST(NetworkClientTest, ReadLargeMessage)
{
    SimpleTestServer server(30014);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30014);

    std::string large_message(100 * 1024, 'L');   // 100KB
    client.send(large_message);

    std::string received;
    EXPECT_NO_THROW({ received = client.read(); });

    EXPECT_EQ(received, large_message);
}

TEST(NetworkClientTest, ReadMessageWithSpecialCharacters)
{
    SimpleTestServer server(30015);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30015);

    std::string special_message = "Special: \n\t\r\"'\\";
    client.send(special_message);

    std::string received = client.read();
    EXPECT_EQ(received, special_message);
}

TEST(NetworkClientTest, ReadMessageWithUnicode)
{
    SimpleTestServer server(30016);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30016);

    std::string unicode_message = "Unicode: 你好世界 🎮";
    client.send(unicode_message);

    std::string received = client.read();
    EXPECT_EQ(received, unicode_message);
}

TEST(NetworkClientTest, ReadMultipleMessages)
{
    SimpleTestServer server(30017);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30017);

    for (int i = 0; i < 5; ++i) {
        std::string test_message = "Message " + std::to_string(i);
        client.send(test_message);

        std::string received = client.read();
        EXPECT_EQ(received, test_message);
    }
}

// ========== Round-trip Tests ==========

TEST(NetworkClientTest, RoundTripSimpleMessage)
{
    SimpleTestServer server(30018);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30018);

    std::string original = "Round trip test";
    client.send(original);
    std::string received = client.read();

    EXPECT_EQ(original, received);
}

TEST(NetworkClientTest, RoundTripJSONMessage)
{
    SimpleTestServer server(30019);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30019);

    std::string json = R"({"player":"Alice","action":"DRAW_CARD","count":3})";
    client.send(json);
    std::string received = client.read();

    EXPECT_EQ(json, received);
}

TEST(NetworkClientTest, RoundTripMultipleMessages)
{
    SimpleTestServer server(30020);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30020);

    std::vector<std::string> messages = {"First message", "Second message with 123", "Third: special chars \n\t", "Fourth: 你好", ""};

    for (const auto &msg : messages) {
        client.send(msg);
        std::string received = client.read();
        EXPECT_EQ(msg, received);
    }
}

TEST(NetworkClientTest, RoundTripLargeMessages)
{
    SimpleTestServer server(30021);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30021);

    for (size_t size : {1024, 10240, 102400}) {
        std::string large_msg(size, 'Z');
        client.send(large_msg);
        std::string received = client.read();
        EXPECT_EQ(large_msg.size(), received.size());
        EXPECT_EQ(large_msg, received);
    }
}

// ========== Edge Cases ==========

TEST(NetworkClientTest, SendBeforeConnect)
{
    auto callback = [](std::string message) {};
    NetworkClient client(callback);

    // Should throw because not connected
    EXPECT_THROW({ client.send("Message before connect"); }, std::exception);
}

TEST(NetworkClientTest, ReadBeforeConnect)
{
    auto callback = [](std::string message) {};
    NetworkClient client(callback);

    // Should throw because not connected
    EXPECT_THROW({ client.read(); }, std::exception);
}

TEST(NetworkClientTest, MultipleConnectAttempts)
{
    SimpleTestServer server1(30022);
    SimpleTestServer server2(30023);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);

    // First connection should succeed
    EXPECT_NO_THROW({ client.connect("127.0.0.1", 30022); });

    // Second connection attempt should also work (replaces first connection)
    EXPECT_NO_THROW({ client.connect("127.0.0.1", 30023); });
}

TEST(NetworkClientTest, RapidSendAndRead)
{
    SimpleTestServer server(30025);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30025);

    // Rapidly send and read 100 messages
    for (int i = 0; i < 100; ++i) {
        std::string msg = std::to_string(i);
        client.send(msg);
        std::string received = client.read();
        EXPECT_EQ(msg, received);
    }
}

TEST(NetworkClientTest, VeryLongMessageContent)
{
    SimpleTestServer server(30026);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30026);

    // Create a complex message with various characters
    std::string complex_message;
    for (int i = 0; i < 1000; ++i) {
        complex_message += "Line " + std::to_string(i) + ": Hello World! 你好世界 🎮\n";
    }

    client.send(complex_message);
    std::string received = client.read();
    EXPECT_EQ(complex_message, received);
}

TEST(NetworkClientTest, SendBinaryData)
{
    SimpleTestServer server(30027);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30027);

    // Create binary data with all byte values
    std::string binary_data;
    for (int i = 1; i < 256; ++i) {   // Skip 0 to avoid null terminator issues
        binary_data += static_cast<char>(i);
    }

    client.send(binary_data);
    std::string received = client.read();
    EXPECT_EQ(binary_data.size(), received.size());
    EXPECT_EQ(binary_data, received);
}

TEST(NetworkClientTest, AlternatingReadAndWrite)
{
    SimpleTestServer server(30028);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30028);

    for (int i = 0; i < 20; ++i) {
        if (i % 2 == 0) {
            client.send("Message " + std::to_string(i));
        }
        else {
            std::string received = client.read();
            EXPECT_FALSE(received.empty());
        }
    }
}

// ========== Protocol Compliance Tests ==========

TEST(NetworkClientTest, MessageLengthPrefixCorrect)
{
    SimpleTestServer server(30029);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto callback = [](std::string message) {};
    NetworkClient client(callback);
    client.connect("127.0.0.1", 30029);

    // Test various message lengths
    for (size_t len : {0, 1, 10, 100, 1000, 10000}) {
        std::string msg(len, 'A');
        client.send(msg);
        std::string received = client.read();
        EXPECT_EQ(len, received.size());
        EXPECT_EQ(msg, received);
    }
}
