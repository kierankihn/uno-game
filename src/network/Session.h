/**
 * @file Session.h
 *
 * @author Yuzhe Guo
 * @date 2025.12.04
 */
#pragma once

#include <asio.hpp>
#include <memory>
#include <queue>

namespace UNO::NETWORK {

    class Session : public std::enable_shared_from_this<Session> {
    private:
        asio::ip::tcp::socket socket_;
        std::function<void(std::string)> callback_;

        std::queue<std::string> messages_;

    public:
        explicit Session(asio::ip::tcp::socket socket);

        /**
         * 开始从网络读取消息
         * @param callback 回调函数
         */
        void start(std::function<void(std::string)> callback);

        /**
         * 发送消息
         * @param message 要发送的消息
         */
        void send(const std::string &message);

    private:
        void doRead();
        void doReadBody(size_t length);
        void doWrite();
    };

}   // namespace UNO::NETWORK