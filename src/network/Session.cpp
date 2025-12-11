/**
 * @file Session.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.12.04
 */
#include "Session.h"

#include <spdlog/spdlog.h>

namespace UNO::NETWORK {
    Session::Session(asio::ip::tcp::socket socket) : socket_(std::move(socket))
    {
        SPDLOG_DEBUG("Session created");
    }

    void Session::start(std::function<void(std::string)> callback)
    {
        SPDLOG_DEBUG("Session started");
        this->callback_ = std::move(callback);
        this->doRead();
    }

    void Session::send(const std::string &message)
    {
        bool writeInProgress = !this->messages_.empty();
        messages_.push(message);
        SPDLOG_DEBUG("Message queued for sending, size: {} bytes, queue size: {}", message.size(), messages_.size());
        if (writeInProgress == false) {
            this->doWrite();
        }
    }

    void Session::doRead()
    {
        auto messageLength = std::make_shared<size_t>(0);
        asio::async_read(socket_,
                         asio::buffer(messageLength.get(), sizeof(size_t)),
                         [this, self = shared_from_this(), messageLength](const asio::error_code &ec, size_t length) {
                             if (!ec) {
                                 SPDLOG_TRACE("Read message header, length: {} bytes", *messageLength);
                                 if (*messageLength <= 10 * 1024 * 1024) {
                                     this->doReadBody(*messageLength);
                                 }
                                 else {
                                     SPDLOG_WARN("Message length {} exceeds limit, skipping", *messageLength);
                                     doRead();
                                 }
                             }
                             else {
                                 SPDLOG_DEBUG("Session read error: {}", ec.message());
                             }
                         });
    }

    void Session::doReadBody(size_t length)
    {
        auto buffer = std::make_shared<std::vector<char>>(length);
        asio::async_read(
            socket_, asio::buffer(*buffer), [this, self = shared_from_this(), buffer, length](const asio::error_code &ec, size_t) {
                if (!ec) {
                    std::string message = {buffer->begin(), buffer->end()};
                    SPDLOG_TRACE("Received message body, size: {} bytes", length);
                    this->callback_(message);
                    doRead();
                }
                else {
                    SPDLOG_DEBUG("Session read body error: {}", ec.message());
                }
            });
    }

    void Session::doWrite()
    {
        auto message = this->messages_.front();
        this->messages_.pop();

        auto length = std::make_shared<size_t>(message.size());
        auto msg    = std::make_shared<std::string>(message);

        SPDLOG_TRACE("Sending message, size: {} bytes", *length);
        std::array<asio::const_buffer, 2> buffers = {asio::buffer(length.get(), sizeof(size_t)), asio::buffer(*msg)};
        asio::async_write(socket_, buffers, [this, self = shared_from_this(), length, msg](const asio::error_code &ec, size_t) {
            if (!ec) {
                SPDLOG_TRACE("Message sent successfully, size: {} bytes", *length);
                if (this->messages_.empty() == false) {
                    this->doWrite();
                }
            }
            else {
                SPDLOG_ERROR("Session write error: {}", ec.message());
            }
        });
    }
}   // namespace UNO::NETWORK
