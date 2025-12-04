/**
 * @file UnoServer.h
 *
 * @author Yuzhe Guo
 * @date 2025.12.01
 */
#pragma once
#include "../game/GameState.h"
#include "../network/NetworkServer.h"

namespace UNO::SERVER {

    class UnoServer {
    private:
        GAME::ServerGameState serverGameState_;
        NETWORK::NetworkServer networkServer_;

        size_t playerCount;
        std::map<size_t, size_t> gameIdToNetworkId;
        std::map<size_t, size_t> networkIdToGameId;
        std::map<size_t, bool> isReadyToStart;

    private:
        /**
         * 处理玩家消息
         * @param playerId 玩家 ID
         * @param message 玩家消息
         */
        void handlePlayerMessage(size_t playerId, const std::string &message);

        /**
         * 开始游戏
         */
        void handleStartGame();

        /**
         * 处理玩家摸牌事件
         * @param playerId 摸牌的玩家的网络 ID
         */
        void handleDrawCard(size_t playerId);

        /**
         * 处理玩家出牌事件
         * @param playerId 出牌的玩家的网络 ID
         * @param card 玩家出的牌
         */
        void handlePlayCard(size_t playerId, GAME::Card card);

        /**
         * 处理游戏结束事件
         */
        void handleEndGame();

    public:
        explicit UnoServer(uint16_t port = 10001);

        /**
         * 启动服务器
         */
        void run();
    };

}   // namespace UNO::SERVER