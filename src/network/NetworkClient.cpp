/**
 * @file NetworkClient.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.28
 */
#include "NetworkClient.h"

#include <asio/connect.hpp>
#include <memory>
#include <utility>

namespace UNO::NETWORK {
    NetworkClient::NetworkClient(std::function<void(std::string)> callback) : callback_(std::move(callback)) {}

    void NetworkClient::connect(const std::string &host, uint16_t port)
    {
        asio::ip::tcp::socket socket(io_context_);

        asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        this->session_ = std::make_shared<Session>(std::move(socket));
        this->session_->start(callback_);
    }


    void NetworkClient::send(const std::string &message) const
    {
        this->session_->send(message);
    }
}   // namespace UNO::NETWORK
