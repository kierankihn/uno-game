/**
 * @file NetworkServer.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.25
 */
#include "NetworkServer.h"

#include <utility>

namespace UNO::NETWORK {
    void NetworkServer::accept()
    {
        this->acceptor_.async_accept([this](const asio::error_code &ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                this->addPlayer(std::move(socket));
                accept();
            }
        });
    }

    NetworkServer::NetworkServer(uint16_t port, std::function<void(size_t, std::string)> callback) :
        acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), playerCount(0), callback_(std::move(callback))
    {
        accept();
    }

    void NetworkServer::addPlayer(asio::ip::tcp::socket socket)
    {
        std::lock_guard<std::mutex> lock(this->mutex_);
        size_t playerId           = this->playerCount;
        this->sessions_[playerId] = std::make_shared<Session>(std::move(socket));
        this->sessions_[playerId]->start([this, playerId](std::string message) { this->callback_(playerId, std::move(message)); });
        this->playerCount++;
    }

    void NetworkServer::send(size_t id, const std::string &message)
    {
        std::lock_guard<std::mutex> lock(this->mutex_);
        if (this->sessions_.contains(id) == false) {
            throw std::invalid_argument("Player session not found");
        }
        this->sessions_[id]->send(message);
    }

    void NetworkServer::run()
    {
        this->io_context_.run();
    }

    void NetworkServer::stop()
    {
        this->acceptor_.close();
        this->io_context_.stop();
    }
}   // namespace UNO::NETWORK