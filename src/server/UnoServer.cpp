/**
 * @file UnoServer.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.12.01
 */
#include "UnoServer.h"

#include "../network/MessageSerializer.h"

namespace UNO::SERVER {
    UnoServer::UnoServer(uint16_t port) :
        networkServer_(port, [this](int playerId, const std::string &message) { this->handlePlayerMessage(playerId, message); }),
        playerCount(0)
    {
    }

    void UnoServer::handlePlayerMessage(size_t playerId, const std::string &message)
    {
        auto playerMessage = NETWORK::MessageSerializer::deserialize(message);

        if (playerMessage.getMessageStatus() == NETWORK::MessageStatus::OK) {
            if (playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::JOIN_GAME) {
                auto playerName = std::get<NETWORK::JoinGamePayload>(playerMessage.getMessagePayload()).playerName;

                this->networkIdToGameId[playerId]          = this->playerCount;
                this->gameIdToNetworkId[this->playerCount] = playerId;
                this->playerCount++;
                this->serverGameState_.addPlayer(GAME::ServerPlayerState{playerName, 0, false});
            }
            if (playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::START_GAME) {
                this->isReadyToStart[networkIdToGameId[playerId]] = true;

                for (size_t i = 0; i <= this->playerCount; i++) {
                    if (i == this->playerCount) {
                        this->handleStartGame();
                        break;
                    }
                    if (isReadyToStart[i] == false) {
                        break;
                    }
                }
            }
            if (playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::INIT_GAME
                || playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::END_GAME) {
                throw std::invalid_argument("Invalid message payload type from client");
            }
            if (this->networkIdToGameId.at(playerId)
                != this->serverGameState_.getCurrentPlayer() - this->serverGameState_.getPlayers().begin()) {
                throw std::invalid_argument("Invalid player message: not this player's turn");
            }
            if (playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::DRAW_CARD) {
                this->handleDrawCard(playerId);
            }
            if (playerMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::PLAY_CARD) {
                this->handlePlayCard(playerId, std::get<NETWORK::PlayCardPayload>(playerMessage.getMessagePayload()).card);
            }
        }
    }

    void UnoServer::handleStartGame()
    {
        serverGameState_.init();
        std::vector<NETWORK::PlayerPublicState> players;
        players.reserve(serverGameState_.getPlayers().size());
        for (const auto &player : serverGameState_.getPlayers()) {
            players.push_back({player.getName(), player.getRemainingCardCount(), player.getIsUno()});
        }
        size_t currentPlayerIndex = static_cast<size_t>(serverGameState_.getCurrentPlayer() - serverGameState_.getPlayers().begin());
        for (size_t i = 0; i < playerCount; i++) {
            NETWORK::InitGamePayload payload = {
                i, players, serverGameState_.getDiscardPile(), serverGameState_.getPlayers()[i].getCards(), currentPlayerIndex};
            this->networkServer_.send(
                gameIdToNetworkId.at(i),
                NETWORK::MessageSerializer::serialize({NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::INIT_GAME, payload}));
        }
    }

    void UnoServer::handleDrawCard(size_t playerId)
    {
        auto cards = this->serverGameState_.updateStateByDraw();
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
        NETWORK::EndGamePayload payload{};
        auto message = NETWORK::MessageSerializer::serialize({NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::END_GAME, payload});

        for (size_t i = 0; i < playerCount; i++) {
            this->networkServer_.send(gameIdToNetworkId.at(i), message);
        }

        this->serverGameState_.reset();

        for (size_t i = 0; i < playerCount; i++) {
            this->isReadyToStart[i] = false;
        }
    }

    void UnoServer::run()
    {
        this->networkServer_.run();
    }

}   // namespace UNO::SERVER
