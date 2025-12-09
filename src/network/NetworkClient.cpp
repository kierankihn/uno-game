/**
 * @file NetworkClient.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.28
 */
#include "NetworkClient.h"

#include <asio/connect.hpp>
#include <iostream>
#include <memory>
#include <utility>

namespace UNO::NETWORK {
    NetworkClient::NetworkClient(std::function<void()> onConnect, std::function<void(std::string)> callback) :
        onConnected_(std::move(onConnect)), callback_(std::move(callback)), workGuard_(asio::make_work_guard(io_context_))
    {
    }

    void NetworkClient::connect(const std::string &host, uint16_t port)
    {
        auto socket   = std::make_shared<asio::ip::tcp::socket>(io_context_);
        auto resolver = std::make_shared<asio::ip::tcp::resolver>(io_context_);
        resolver->async_resolve(host,
                                std::to_string(port),
                                [this, resolver, socket](const asio::error_code &ec, const asio::ip::tcp::resolver::results_type &results) {
                                    if (!ec) {
                                        asio::async_connect(
                                            *socket, results, [this, socket](const asio::error_code &ec, const asio::ip::tcp::endpoint &) {
                                                if (!ec) {
                                                    this->session_ = std::make_shared<Session>(std::move(*socket));
                                                    this->session_->start(callback_);
                                                    this->onConnected_();
                                                }
                                            });
                                    }
                                });
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
        this->workGuard_.reset();
        this->io_context_.stop();
    }
}   // namespace UNO::NETWORK
