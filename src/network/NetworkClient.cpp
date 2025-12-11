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
#include <spdlog/spdlog.h>
#include <utility>

namespace UNO::NETWORK {
    NetworkClient::NetworkClient(std::function<void()> onConnect, std::function<void(std::string)> callback) :
        onConnected_(std::move(onConnect)), callback_(std::move(callback)), workGuard_(asio::make_work_guard(io_context_))
    {
        SPDLOG_DEBUG("NetworkClient initialized");
    }

    void NetworkClient::connect(const std::string &host, uint16_t port)
    {
        SPDLOG_INFO("Connecting to {}:{}", host, port);
        auto socket   = std::make_shared<asio::ip::tcp::socket>(io_context_);
        auto resolver = std::make_shared<asio::ip::tcp::resolver>(io_context_);
        resolver->async_resolve(
            host,
            std::to_string(port),
            [this, resolver, socket, host, port](const asio::error_code &ec, const asio::ip::tcp::resolver::results_type &results) {
                if (!ec) {
                    SPDLOG_DEBUG("Resolved host {}:{}", host, port);
                    asio::async_connect(
                        *socket, results, [this, socket, host, port](const asio::error_code &ec, const asio::ip::tcp::endpoint &) {
                            if (!ec) {
                                SPDLOG_INFO("Successfully connected to {}:{}", host, port);
                                this->session_ = std::make_shared<Session>(std::move(*socket));
                                this->session_->start(callback_);
                                this->onConnected_();
                            }
                            else {
                                SPDLOG_ERROR("Failed to connect to {}:{}: {}", host, port, ec.message());
                            }
                        });
                }
                else {
                    SPDLOG_ERROR("Failed to resolve host {}:{}: {}", host, port, ec.message());
                }
            });
    }


    void NetworkClient::send(const std::string &message)
    {
        SPDLOG_DEBUG("Queueing message to send, size: {} bytes", message.size());
        asio::post(io_context_, [session = this->session_, message]() { session->send(message); });
    }

    void NetworkClient::run()
    {
        SPDLOG_INFO("NetworkClient starting io_context loop");
        this->io_context_.run();
        SPDLOG_INFO("NetworkClient io_context loop stopped");
    }

    void NetworkClient::stop()
    {
        SPDLOG_INFO("NetworkClient stopping");
        this->workGuard_.reset();
        this->io_context_.stop();
    }
}   // namespace UNO::NETWORK
