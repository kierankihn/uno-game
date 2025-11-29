/**
 * @file NetworkClient.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.28
 */
#include "NetworkClient.h"

#include <asio/connect.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>
#include <utility>

namespace UNO::NETWORK {
    NetworkClient::NetworkClient(std::function<void(std::string)> callback) : socket_(io_context_), callback_(std::move(callback)) {}

    void NetworkClient::connect(const std::string &host, uint16_t port)
    {
        this->disconnect();
        asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        asio::connect(socket_, endpoints.begin(), endpoints.end());
    }

    void NetworkClient::disconnect()
    {
        if (socket_.is_open()) {
            this->socket_.close();
        }
    }


    void NetworkClient::send(const std::string &message)
    {
        size_t length = message.size();
        asio::write(this->socket_, asio::buffer(&length, sizeof(length)));
        asio::write(this->socket_, asio::buffer(message));
    }

    std::string NetworkClient::read()
    {
        size_t length;
        asio::read(this->socket_, asio::buffer(&length, sizeof(length)));
        std::vector<char> buffer(length);
        asio::read(this->socket_, asio::buffer(buffer));
        return {buffer.begin(), buffer.end()};
    }
}   // namespace UNO::NETWORK
