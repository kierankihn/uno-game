/**
 * @file NetworkServerTest.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.28
 */

#include "../../../src/network/NetworkServer.h"

#include <asio.hpp>
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <mutex>
#include <set>
#include <thread>
#include <utility>

using namespace UNO::NETWORK;

// ========== NetworkServer Constructor Tests ==========

TEST(NetworkServerTest, ConstructorWithValidPort)
{
    auto callback = [](size_t player_id, std::string message) {};

    EXPECT_NO_THROW({ NetworkServer server(20001, callback); });
}

TEST(NetworkServerTest, ConstructorWithZeroPort)
{
    auto callback = [](size_t player_id, std::string message) {};

    // Port 0 should let OS assign a port
    EXPECT_NO_THROW({ NetworkServer server(0, callback); });
}

TEST(NetworkServerTest, ConstructorWithHighPort)
{
    auto callback = [](size_t player_id, std::string message) {};

    EXPECT_NO_THROW({ NetworkServer server(65535, callback); });
}

// ========== NetworkServer AddPlayer Tests ==========

TEST(NetworkServerTest, AddPlayerSinglePlayer)
{
    std::atomic<bool> callback_called{false};
    std::atomic<size_t> received_player_id{999};
    std::string received_message;
    std::mutex msg_mutex;

    auto callback = [&callback_called, &received_player_id, &received_message, &msg_mutex](size_t player_id, std::string message) {
        callback_called    = true;
        received_player_id = player_id;
        std::lock_guard<std::mutex> lock(msg_mutex);
        received_message = std::move(message);
    };

    NetworkServer server(20002, callback);

    // Start server in background thread
    std::thread server_thread([&server]() { server.run(); });

    // Give server time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Connect a client
    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "20002");
    asio::connect(socket, endpoints);

    // Give time for connection to be established
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send a message to trigger callback
    std::string test_message = "Hello Server";
    size_t length            = test_message.size();
    asio::write(socket, asio::buffer(&length, sizeof(length)));
    asio::write(socket, asio::buffer(test_message));

    // Give time for message to be received
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify callback was called
    EXPECT_TRUE(callback_called);
    EXPECT_EQ(received_player_id, 0);
    {
        std::lock_guard<std::mutex> lock(msg_mutex);
        EXPECT_EQ(received_message, "Hello Server");
    }

    socket.close();
    io_context.stop();

    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

TEST(NetworkServerTest, AddPlayerMultiplePlayers)
{
    std::mutex mutex;
    size_t message_count{0};
    std::set<size_t> received_player_ids;

    auto callback = [&message_count, &received_player_ids, &mutex](size_t player_id, std::string message) {
        std::lock_guard<std::mutex> lock(mutex);
        message_count++;
        received_player_ids.insert(player_id);
    };

    NetworkServer server(20003, callback);

    std::thread server_thread([&server]() { server.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Connect multiple clients
    std::vector<std::unique_ptr<asio::io_context>> contexts;
    std::vector<std::unique_ptr<asio::ip::tcp::socket>> sockets;

    for (int i = 0; i < 3; ++i) {
        contexts.push_back(std::make_unique<asio::io_context>());
        sockets.push_back(std::make_unique<asio::ip::tcp::socket>(*contexts.back()));

        asio::ip::tcp::resolver resolver(*contexts.back());
        auto endpoints = resolver.resolve("127.0.0.1", "20003");
        asio::connect(*sockets.back(), endpoints);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Each client sends a message
    for (int i = 0; i < 3; ++i) {
        std::string msg = "Message from client " + std::to_string(i);
        size_t length   = msg.size();
        asio::write(*sockets[i], asio::buffer(&length, sizeof(length)));
        asio::write(*sockets[i], asio::buffer(msg));
    }

    // Give time for messages to be received
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    // Verify all callbacks were called
    EXPECT_EQ(message_count, 3);
    {
        std::lock_guard<std::mutex> lock(mutex);
        EXPECT_EQ(received_player_ids.size(), 3);
        EXPECT_TRUE(received_player_ids.contains(0));
        EXPECT_TRUE(received_player_ids.contains(1));
        EXPECT_TRUE(received_player_ids.contains(2));
    }

    // Cleanup
    for (auto &socket : sockets) {
        socket->close();
    }
    for (auto &context : contexts) {
        context->stop();
    }

    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

// ========== NetworkServer Send Tests ==========

TEST(NetworkServerTest, SendMessageToValidPlayer)
{
    auto callback = [](size_t player_id, std::string message) {};

    NetworkServer server(20004, callback);

    std::thread server_thread([&server]() { server.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Connect a client
    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "20004");
    asio::connect(socket, endpoints);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send message to player 0
    EXPECT_NO_THROW({ server.send(0, "test message"); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Read the message from client side to verify it was sent
    std::atomic<bool> message_received{false};
    std::string received_msg;
    std::thread read_thread([&socket, &message_received, &received_msg]() {
        try {
            size_t length;
            asio::read(socket, asio::buffer(&length, sizeof(length)));
            std::vector<char> buffer(length);
            asio::read(socket, asio::buffer(buffer));
            received_msg = std::string(buffer.begin(), buffer.end());
            if (received_msg == "test message") {
                message_received = true;
            }
        }
        catch (...) {
        }
    });

    read_thread.join();
    EXPECT_TRUE(message_received);
    EXPECT_EQ(received_msg, "test message");

    socket.close();
    io_context.stop();
    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

TEST(NetworkServerTest, SendMessageToInvalidPlayer)
{
    auto callback = [](size_t player_id, std::string message) {};

    NetworkServer server(20005, callback);

    // Try to send to non-existent player
    EXPECT_THROW({ server.send(999, "test message"); }, std::invalid_argument);
}

TEST(NetworkServerTest, SendEmptyMessage)
{
    auto callback = [](size_t player_id, std::string message) {};

    NetworkServer server(20006, callback);

    std::thread server_thread([&server]() { server.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "20006");
    asio::connect(socket, endpoints);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_NO_THROW({ server.send(0, ""); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Verify empty message was received
    std::atomic<bool> message_received{false};
    std::thread read_thread([&socket, &message_received]() {
        try {
            size_t length;
            asio::read(socket, asio::buffer(&length, sizeof(length)));
            if (length == 0) {
                message_received = true;
            }
        }
        catch (...) {
        }
    });

    read_thread.join();
    EXPECT_TRUE(message_received);

    socket.close();
    io_context.stop();
    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

TEST(NetworkServerTest, SendLargeMessage)
{
    auto callback = [](size_t player_id, std::string message) {};

    NetworkServer server(20007, callback);

    std::thread server_thread([&server]() { server.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "20007");
    asio::connect(socket, endpoints);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string large_message(1024 * 1024, 'A');   // 1MB message
    EXPECT_NO_THROW({ server.send(0, large_message); });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify large message was received
    std::atomic<bool> message_received{false};
    std::thread read_thread([&socket, &message_received, &large_message]() {
        try {
            size_t length;
            asio::read(socket, asio::buffer(&length, sizeof(length)));
            std::vector<char> buffer(length);
            asio::read(socket, asio::buffer(buffer));
            std::string received(buffer.begin(), buffer.end());
            if (received == large_message) {
                message_received = true;
            }
        }
        catch (...) {
        }
    });

    read_thread.join();
    EXPECT_TRUE(message_received);

    socket.close();
    io_context.stop();
    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

TEST(NetworkServerTest, SendMessageWithSpecialCharacters)
{
    auto callback = [](size_t player_id, std::string message) {};

    NetworkServer server(20008, callback);

    std::thread server_thread([&server]() { server.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "20008");
    asio::connect(socket, endpoints);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string special_msg = "Special chars: \n\t\r\"'\\";
    EXPECT_NO_THROW({ server.send(0, special_msg); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Verify message with special characters was received
    std::atomic<bool> message_received{false};
    std::thread read_thread([&socket, &message_received, &special_msg]() {
        try {
            size_t length;
            asio::read(socket, asio::buffer(&length, sizeof(length)));
            std::vector<char> buffer(length);
            asio::read(socket, asio::buffer(buffer));
            std::string received(buffer.begin(), buffer.end());
            if (received == special_msg) {
                message_received = true;
            }
        }
        catch (...) {
        }
    });

    read_thread.join();
    EXPECT_TRUE(message_received);

    socket.close();
    io_context.stop();
    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

TEST(NetworkServerTest, SendMessageWithUnicode)
{
    auto callback = [](size_t player_id, std::string message) {};

    NetworkServer server(20009, callback);

    std::thread server_thread([&server]() { server.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "20009");
    asio::connect(socket, endpoints);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string unicode_msg = "Unicode: 你好世界 🎮";
    EXPECT_NO_THROW({ server.send(0, unicode_msg); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Verify Unicode message was received
    std::atomic<bool> message_received{false};
    std::thread read_thread([&socket, &message_received, &unicode_msg]() {
        try {
            size_t length;
            asio::read(socket, asio::buffer(&length, sizeof(length)));
            std::vector<char> buffer(length);
            asio::read(socket, asio::buffer(buffer));
            std::string received(buffer.begin(), buffer.end());
            if (received == unicode_msg) {
                message_received = true;
            }
        }
        catch (...) {
        }
    });

    read_thread.join();
    EXPECT_TRUE(message_received);

    socket.close();
    io_context.stop();
    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

// ========== Session Tests ==========

TEST(SessionTest, SessionCreation)
{
    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);

    EXPECT_NO_THROW({ Session session(0, std::move(socket)); });
}

TEST(SessionTest, SessionStartWithCallback)
{
    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);

    auto session = std::make_shared<Session>(0, std::move(socket));

    auto callback = [](size_t player_id, std::string message) {};

    EXPECT_NO_THROW({ session->start(callback); });
}

// ========== Concurrent Access Tests ==========

TEST(NetworkServerTest, ConcurrentSendToSamePlayer)
{
    auto callback = [](size_t player_id, std::string message) {};

    NetworkServer server(20010, callback);

    std::thread server_thread([&server]() { server.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "20010");
    asio::connect(socket, endpoints);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Multiple threads sending to same player
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&server, i]() { server.send(0, "Message " + std::to_string(i)); });
    }

    for (auto &t : threads) {
        t.join();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    socket.close();
    io_context.stop();
    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

TEST(NetworkServerTest, ConcurrentSendToDifferentPlayers)
{
    auto callback = [](size_t player_id, std::string message) {};

    NetworkServer server(20011, callback);

    std::thread server_thread([&server]() { server.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Connect multiple clients
    std::vector<std::unique_ptr<asio::io_context>> contexts;
    std::vector<std::unique_ptr<asio::ip::tcp::socket>> sockets;

    for (int i = 0; i < 3; ++i) {
        contexts.push_back(std::make_unique<asio::io_context>());
        sockets.push_back(std::make_unique<asio::ip::tcp::socket>(*contexts.back()));

        asio::ip::tcp::resolver resolver(*contexts.back());
        auto endpoints = resolver.resolve("127.0.0.1", "20011");
        asio::connect(*sockets.back(), endpoints);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Multiple threads sending to different players
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([&server, i]() { server.send(i, "Message to player " + std::to_string(i)); });
    }

    for (auto &t : threads) {
        t.join();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Cleanup
    for (auto &socket : sockets) {
        socket->close();
    }
    for (auto &context : contexts) {
        context->stop();
    }

    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

// ========== Edge Cases ==========

TEST(NetworkServerTest, MultipleMessagesToSamePlayer)
{
    std::atomic<size_t> callback_count{0};
    auto callback = [&callback_count](size_t player_id, std::string message) { callback_count++; };

    NetworkServer server(20012, callback);

    std::thread server_thread([&server]() { server.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "20012");
    asio::connect(socket, endpoints);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send multiple messages from server
    for (int i = 0; i < 10; ++i) {
        EXPECT_NO_THROW({ server.send(0, "Message " + std::to_string(i)); });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Client sends messages back to trigger callbacks
    for (int i = 0; i < 5; ++i) {
        std::string msg = "Client message " + std::to_string(i);
        size_t length   = msg.size();
        asio::write(socket, asio::buffer(&length, sizeof(length)));
        asio::write(socket, asio::buffer(msg));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify callbacks were called
    EXPECT_EQ(callback_count, 5);

    socket.close();
    io_context.stop();
    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

TEST(NetworkServerTest, SendAfterPlayerDisconnect)
{
    auto callback = [](size_t player_id, std::string message) {};

    NetworkServer server(20013, callback);

    std::thread server_thread([&server]() { server.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    {
        asio::io_context io_context;
        asio::ip::tcp::socket socket(io_context);
        asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "20013");
        asio::connect(socket, endpoints);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Socket goes out of scope and closes
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Player 0 should still exist in the session map even after disconnect
    EXPECT_NO_THROW({ server.send(0, "Message after disconnect"); });

    server.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}
