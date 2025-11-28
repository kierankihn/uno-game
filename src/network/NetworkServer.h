/**
 * @file NetworkServer.h
 *
 * @author Yuzhe Guo
 * @date 2025.11.25
 */
#pragma once
#include "Message.h"


#include <asio.hpp>
#include <map>
#include <memory>

namespace UNO::NETWORK {

    class Session : public std::enable_shared_from_this<Session> {
    private:
        size_t player_id_;
        asio::ip::tcp::socket socket_;
        std::function<void(size_t, std::string)> callback_;

    public:
        Session(size_t player_id, asio::ip::tcp::socket socket);

        /**
         * 开始从网络读取消息
         * @param callback 回调函数
         */
        void start(std::function<void(size_t, std::string)> callback);

        /**
         * 发送消息
         * @param message 要发送的消息
         */
        void send(const std::string &message);

    private:
        void read();
        void readBody(size_t length);
    };

    class NetworkServer {
    private:
        asio::io_context io_context_;
        asio::ip::tcp::acceptor acceptor_;
        size_t playerCount;
        std::map<size_t, std::shared_ptr<Session>> sessions_;
        std::function<void(size_t, std::string)> callback_;
        std::mutex mutex_;

    public:
        explicit NetworkServer(uint16_t port, std::function<void(size_t, std::string)> callback);

        /**
         * 添加玩家
         * @param socket 玩家的连接 socket
         */
        void addPlayer(asio::ip::tcp::socket socket);

        /**
         * 向玩家发送消息
         * @param id 要发送到的玩家 id
         * @param message 要发送的消息
         */
        void send(size_t id, const std::string &message);

        /**
         * 开始网络进程
         */
        void run();

    private:
        void accept();
    };

}   // namespace UNO::NETWORK