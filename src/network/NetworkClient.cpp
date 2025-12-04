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
        io_context_.restart();
        asio::ip::tcp::socket socket(io_context_);
        asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        asio::connect(socket, endpoints);
        this->session_ = std::make_shared<Session>(std::move(socket));
        this->session_->start(callback_);
    }


    void NetworkClient::send(const std::string &message)
    {
        asio::post(io_context_, [session = this->session_, message]() { session->send(message); });
    }

    void NetworkClient::run()
    {
        this->io_context_.run();
    }

    void NetworkClient::stop()
    {
        this->io_context_.stop();
    }
}   // namespace UNO::NETWORK
