/**
 * @file NetworkServer.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.25
 */
#include "NetworkServer.h"

#include <spdlog/spdlog.h>
#include <utility>

namespace UNO::NETWORK {
    void NetworkServer::accept()
    {
        this->acceptor_.async_accept([this](const asio::error_code &ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                SPDLOG_INFO("Accepted new connection from {}", socket.remote_endpoint().address().to_string());
                this->addPlayer(std::move(socket));
                accept();
            }
            else {
                SPDLOG_ERROR("Accept error: {}", ec.message());
            }
        });
    }

    NetworkServer::NetworkServer(uint16_t port, std::function<void(size_t, std::string)> callback) :
        acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), playerCount(0), callback_(std::move(callback))
    {
        SPDLOG_INFO("NetworkServer initialized on port {}", port);
        accept();
    }

    void NetworkServer::addPlayer(asio::ip::tcp::socket socket)
    {
        std::lock_guard<std::mutex> lock(this->mutex_);
        size_t playerId           = this->playerCount;
        this->sessions_[playerId] = std::make_shared<Session>(std::move(socket));
        this->sessions_[playerId]->start([this, playerId](std::string message) { this->callback_(playerId, std::move(message)); });
        this->playerCount++;
        SPDLOG_INFO("Player {} added, total players: {}", playerId, this->playerCount);
    }

    void NetworkServer::send(size_t id, const std::string &message)
    {
        std::lock_guard<std::mutex> lock(this->mutex_);
        if (this->sessions_.contains(id) == false) {
            SPDLOG_ERROR("Failed to send message: Player session {} not found", id);
            throw std::invalid_argument("Player session not found");
        }
        SPDLOG_DEBUG("Sending message to player {}, size: {} bytes", id, message.size());
        this->sessions_[id]->send(message);
    }

    void NetworkServer::run()
    {
        SPDLOG_INFO("NetworkServer starting io_context loop");
        this->io_context_.run();
        SPDLOG_INFO("NetworkServer io_context loop stopped");
    }

    void NetworkServer::stop()
    {
        SPDLOG_INFO("NetworkServer stopping");
        this->acceptor_.close();
        this->io_context_.stop();
    }
}   // namespace UNO::NETWORK
