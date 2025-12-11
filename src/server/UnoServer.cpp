/**
 * @file UnoServer.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.12.01
 */
#include "UnoServer.h"

#include "../network/MessageSerializer.h"
#include <spdlog/spdlog.h>

namespace UNO::SERVER {
    UnoServer::UnoServer(uint16_t port) :
        networkServer_(port, [this](size_t playerId, const std::string &message) { this->handlePlayerMessage(playerId, message); }),
        playerCount(0)
    {
        SPDLOG_INFO("UnoServer initialized on port {}", port);
    }

    void UnoServer::handlePlayerMessage(size_t playerId, const std::string &message)
    {
        auto playerMessage = NETWORK::MessageSerializer::deserialize(message);

        if (playerMessage.getMessageStatus() == NETWORK::MessageStatus::OK) {
            SPDLOG_DEBUG("Processing message from player {}, type: {}", playerId, static_cast<int>(playerMessage.getMessagePayloadType()));

            if (playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::JOIN_GAME) {
                auto playerName = std::get<NETWORK::JoinGamePayload>(playerMessage.getMessagePayload()).playerName;

                this->networkIdToGameId[playerId]          = this->playerCount;
                this->gameIdToNetworkId[this->playerCount] = playerId;
                this->playerCount++;
                this->serverGameState_.addPlayer(GAME::ServerPlayerState{playerName, 0, false});
                SPDLOG_INFO("Player {} joined with name '{}', game ID: {}", playerId, playerName, this->playerCount - 1);
            }
            else if (playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::START_GAME) {
                this->isReadyToStart[networkIdToGameId[playerId]] = true;
                SPDLOG_INFO("Player {} (game ID: {}) is ready to start", playerId, networkIdToGameId[playerId]);

                for (size_t i = 0; i <= this->playerCount; i++) {
                    if (i == this->playerCount) {
                        SPDLOG_INFO("All {} players ready, starting game", this->playerCount);
                        this->handleStartGame();
                        break;
                    }
                    if (isReadyToStart[i] == false) {
                        break;
                    }
                }
            }
            else if (playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::INIT_GAME
                     || playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::END_GAME) {
                SPDLOG_ERROR(
                    "Invalid message payload type from client {}: {}", playerId, static_cast<int>(playerMessage.getMessagePayloadType()));
                throw std::invalid_argument("Invalid message payload type from client");
            }
            else if (this->serverGameState_.getServerGameStage() == GAME::ServerGameStage::IN_GAME
                     && this->networkIdToGameId.at(playerId) != this->serverGameState_.getCurrentPlayerId()) {
                SPDLOG_WARN("Player {} sent message but it's not their turn (current: {})",
                            this->networkIdToGameId.at(playerId),
                            this->serverGameState_.getCurrentPlayerId());
                throw std::invalid_argument("Invalid player message: not this player's turn");
            }
            else if (playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::DRAW_CARD) {
                SPDLOG_INFO("Player {} (game ID: {}) draws card", playerId, this->networkIdToGameId.at(playerId));
                this->handleDrawCard(playerId);
            }
            else if (playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::PLAY_CARD) {
                auto card = std::get<NETWORK::PlayCardPayload>(playerMessage.getMessagePayload()).card;
                SPDLOG_INFO("Player {} (game ID: {}) plays card: color={}, type={}",
                            playerId,
                            this->networkIdToGameId.at(playerId),
                            card.colorToString(),
                            card.typeToString());
                this->handlePlayCard(playerId, card);
            }
        }
        else {
            SPDLOG_ERROR("Received message with error status from player {}", playerId);
        }
    }

    void UnoServer::handleStartGame()
    {
        SPDLOG_INFO("Initializing game state");
        serverGameState_.init();
        std::vector<GAME::ClientPlayerState> players;
        players.reserve(serverGameState_.getPlayers().size());
        for (const auto &player : serverGameState_.getPlayers()) {
            players.emplace_back(player.getName(), player.getRemainingCardCount(), player.getIsUno());
        }
        size_t currentPlayerIndex = serverGameState_.getCurrentPlayerId();
        SPDLOG_INFO("Game started, current player index: {}", currentPlayerIndex);

        for (size_t i = 0; i < playerCount; i++) {
            NETWORK::InitGamePayload payload = {
                i, players, serverGameState_.getDiscardPile(), serverGameState_.getPlayers()[i].getCards(), currentPlayerIndex};
            this->networkServer_.send(
                gameIdToNetworkId.at(i),
                NETWORK::MessageSerializer::serialize({NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::INIT_GAME, payload}));
            SPDLOG_DEBUG("Sent INIT_GAME to player {}", i);
        }
    }

    void UnoServer::handleDrawCard(size_t playerId)
    {
        auto cards = this->serverGameState_.updateStateByDraw();
        SPDLOG_INFO("Player {} drew {} card(s)", this->networkIdToGameId.at(playerId), cards.size());

        for (size_t i = 0; i < playerCount; i++) {
            auto networkId = gameIdToNetworkId.at(i);
            NETWORK::DrawCardPayload payload;
            if (networkId != playerId) {
                payload = {cards.size(), {}};
            }
            else {
                payload = {cards.size(), cards};
            }
            this->networkServer_.send(
                networkId,
                NETWORK::MessageSerializer::serialize({NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::DRAW_CARD, payload}));
        }
    }

    void UnoServer::handlePlayCard(size_t playerId, GAME::Card card)
    {
        this->serverGameState_.updateStateByCard(card);

        // 检查是否有玩家获胜（手牌为空）
        bool gameEnded = false;
        for (const auto &player : this->serverGameState_.getPlayers()) {
            if (player.isEmpty()) {
                gameEnded = true;
                SPDLOG_INFO("Player '{}' wins the game!", player.getName());
                break;
            }
        }

        NETWORK::PlayCardPayload payload = {card};
        auto message = NETWORK::MessageSerializer::serialize({NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::PLAY_CARD, payload});
        for (size_t i = 0; i < playerCount; i++) {
            this->networkServer_.send(gameIdToNetworkId.at(i), message);
        }

        if (gameEnded) {
            this->handleEndGame();
        }
    }

    void UnoServer::handleEndGame()
    {
        SPDLOG_INFO("Game ended, resetting to pre-game state");
        this->serverGameState_.endGame();

        NETWORK::EndGamePayload payload{};
        auto message = NETWORK::MessageSerializer::serialize({NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::END_GAME, payload});

        for (size_t i = 0; i < playerCount; i++) {
            this->networkServer_.send(gameIdToNetworkId.at(i), message);
        }

        for (size_t i = 0; i < playerCount; i++) {
            this->isReadyToStart[i] = false;
        }
    }

    void UnoServer::run()
    {
        SPDLOG_INFO("UnoServer starting");
        this->networkServer_.run();
    }

}   // namespace UNO::SERVER
