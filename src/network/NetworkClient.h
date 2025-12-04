/**
 * @file NetworkClient.h
 *
 * @author Yuzhe Guo
 * @date 2025.11.28
 */
#pragma once
#include "Session.h"

#include <asio/io_context.hpp>

namespace UNO::NETWORK {

    class NetworkClient {
    private:
        asio::io_context io_context_;
        std::function<void(std::string)> callback_;

        std::shared_ptr<Session> session_;

    public:
        explicit NetworkClient(std::function<void(std::string)> callback);

        /**
         * 连接到服务端
         * @param host 服务端地址
         * @param port 服务端端口
         */
        void connect(const std::string &host, uint16_t port);

        /**
         * 向服务端发送消息
         * @param message 要发送的消息
         */
        void send(const std::string &message);

        /**
         * 启动网络事件循环
         */
        void run();

        /**
         * 停止网络事件循环
         */
        void stop();
    };

}   // namespace UNO::NETWORK
