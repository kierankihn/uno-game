/**
 * @file NetworkServer.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.25
 */
#include "NetworkServer.h"

#include <utility>

namespace UNO::NETWORK {
    Session::Session(size_t player_id, asio::ip::tcp::socket socket) : player_id_(player_id), socket_(std::move(socket)) {}

    void Session::start(std::function<void(size_t, std::string)> callback)
    {
        this->callback_ = std::move(callback);
        read();
    }

    void Session::send(const std::string &message)
    {
        auto length                               = std::make_shared<size_t>(message.size());
        auto msg                                  = std::make_shared<std::string>(message);
        std::array<asio::const_buffer, 2> buffers = {asio::buffer(length.get(), sizeof(size_t)), asio::buffer(*msg)};
        asio::async_write(socket_, buffers, [this, self = shared_from_this(), length, msg](const asio::error_code &ec, size_t) {});
    }

    void Session::read()
    {
        auto messageLength = std::make_shared<size_t>(0);
        asio::async_read(socket_,
                         asio::buffer(messageLength.get(), sizeof(size_t)),
                         [this, self = shared_from_this(), messageLength](const asio::error_code &ec, size_t length) {
                             if (!ec) {
                                 if (messageLength > 0 && messageLength <= 10 * 1024 * 1024) {
                                     this->readBody(messageLength);
                                 }
                             }
                         });
    }

    void Session::readBody(size_t length)
    {
        auto buffer = std::make_shared<std::vector<char>>(length);
        asio::async_read(
            socket_, asio::buffer(*buffer), [this, self = shared_from_this(), buffer](const asio::error_code &ec, size_t length) {
                if (!ec) {
                    std::string message = {buffer->begin(), buffer->end()};
                    this->callback_(this->player_id_, message);
                }
            });
    }

    void NetworkServer::accept()
    {
        this->acceptor_.async_accept([this](const asio::error_code &ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                this->addPlayer(std::move(socket));
            }
        });
    }

    NetworkServer::NetworkServer(uint16_t port, std::function<void(size_t, std::string)> callback) :
        acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), playerCount(0), callback_(std::move(callback))
    {
    }

    void NetworkServer::addPlayer(asio::ip::tcp::socket socket)
    {
        std::lock_guard<std::mutex> lock(this->mutex_);
        this->sessions_[this->playerCount] = std::make_shared<Session>(playerCount, std::move(socket));
        this->sessions_[this->playerCount]->start(callback_);
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