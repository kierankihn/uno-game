/**
 * @file
 *
 * @author Yuzhe Guo
 * @date 2025.12.06
 */
#include "UnoClient.h"

#include "../network/Message.h"
#include "../network/MessageSerializer.h"

#include <memory>
#include <spdlog/spdlog.h>
#include <utility>

namespace UNO::CLIENT {
    void UnoClient::handleNetworkConnected()
    {
        SPDLOG_INFO("Connected to server, sending JOIN_GAME message");
        NETWORK::JoinGamePayload messagePayload = {this->clientGameState_->getPlayerName()};
        NETWORK::Message message                = {NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::JOIN_GAME, messagePayload};
        networkClient_->send(NETWORK::MessageSerializer::serialize(message));

        this->clientGameState_->setClientGameStageConnected();
        gameUI_->updateUI(this->clientGameState_);
    }

    void UnoClient::handleNetworkInitGame(const NETWORK::InitGamePayload &payload)
    {
        SPDLOG_INFO("Initializing game: player ID={}, {} players total", payload.playerId, payload.players.size());
        this->clientGameState_->init(payload.players, payload.discardPile, payload.handCard, payload.currentPlayerIndex, payload.playerId);
    }

    void UnoClient::handleNetworkPlayCard(const NETWORK::PlayCardPayload &payload)
    {
        SPDLOG_DEBUG(
            "Received PLAY_CARD: color={}, type={}", static_cast<int>(payload.card.getColor()), static_cast<int>(payload.card.getType()));
        if (clientGameState_->getClientGameStage() == GAME::ClientGameStage::ACTIVE) {
            clientGameState_->play(payload.card);
        }
        clientGameState_->updateStateByCard(payload.card);
    }

    void UnoClient::handleNetworkDrawCard(const NETWORK::DrawCardPayload &payload)
    {
        SPDLOG_DEBUG("Received DRAW_CARD: {} card(s)", payload.drawCount);
        if (clientGameState_->getClientGameStage() == GAME::ClientGameStage::ACTIVE) {
            clientGameState_->draw(payload.cards);
        }
        clientGameState_->updateStateByDraw();
    }

    void UnoClient::handleNetworkEndGame(const NETWORK::EndGamePayload &payload)
    {
        SPDLOG_INFO("Game ended");
        this->clientGameState_->endGame();
    }

    void UnoClient::handleNetworkMessage(const std::string &message)
    {
        auto networkMessage = NETWORK::MessageSerializer::deserialize(message);
        SPDLOG_DEBUG("Received network message, type: {}", static_cast<int>(networkMessage.getMessagePayloadType()));

        if (networkMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::INIT_GAME) {
            this->handleNetworkInitGame(std::get<NETWORK::InitGamePayload>(networkMessage.getMessagePayload()));
        }
        if (networkMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::DRAW_CARD) {
            this->handleNetworkDrawCard(std::get<NETWORK::DrawCardPayload>(networkMessage.getMessagePayload()));
        }
        if (networkMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::PLAY_CARD) {
            this->handleNetworkPlayCard(std::get<NETWORK::PlayCardPayload>(networkMessage.getMessagePayload()));
        }
        if (networkMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::END_GAME) {
            this->handleNetworkEndGame(std::get<NETWORK::EndGamePayload>(networkMessage.getMessagePayload()));
        }

        if (networkMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::EMPTY
            || networkMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::JOIN_GAME
            || networkMessage.getMessagePayloadType() == NETWORK::MessagePayloadType::START_GAME) {
            SPDLOG_ERROR("Invalid message type from server: {}", static_cast<int>(networkMessage.getMessagePayloadType()));
            throw std::invalid_argument("Invalid message type from server");
        }

        gameUI_->updateUI(this->clientGameState_);
    }

    void UnoClient::handlePlayerAction(PlayerAction action)
    {
        SPDLOG_DEBUG("Handling player action, type: {}", static_cast<int>(action.playerActionType));
        if (action.playerActionType == PlayerActionType::CONNECT) {
            this->handlePlayerConnect(std::get<PlayerConnectPayload>(action.payload));
        }
        if (action.playerActionType == PlayerActionType::START_GAME) {
            this->handlePlayerStartGame(std::get<PlayerStartGamePayload>(action.payload));
        }
        if (action.playerActionType == PlayerActionType::PLAY_CARD) {
            this->handlePlayerPlayCard(std::get<PlayerPlayCardPayload>(action.payload));
        }
        if (action.playerActionType == PlayerActionType::DRAW_CARD) {
            this->handlePlayerDrawCard(std::get<PlayerDrawCardPayload>(action.payload));
        }
    }


    void UnoClient::handlePlayerConnect(const PlayerConnectPayload &payload)
    {
        SPDLOG_INFO("Player '{}' connecting to {}:{}", payload.playerName, payload.host, payload.port);
        clientGameState_->setPlayerName(payload.playerName);
        networkClient_->connect(payload.host, payload.port);
    }


    void UnoClient::handlePlayerStartGame(PlayerStartGamePayload payload)
    {
        SPDLOG_INFO("Player requests to start game");
        NETWORK::StartGamePayload messagePayload = {};
        NETWORK::Message message                 = {NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::START_GAME, messagePayload};
        networkClient_->send(NETWORK::MessageSerializer::serialize(message));
    }

    void UnoClient::handlePlayerPlayCard(PlayerPlayCardPayload payload)
    {
        auto cards = this->clientGameState_->getCards();
        auto card  = cards.begin();
        for (size_t i = 0; i < payload.id; i++) {
            card = std::next(card);
        }

        if ((card->getType() == GAME::CardType::WILD || card->getType() == GAME::CardType::WILDDRAWFOUR)
            && card->getColor() != GAME::CardColor::RED) {
            SPDLOG_ERROR("Invalid wild card played by player: card doesn't have color set");
            throw std::invalid_argument("Invalid card played by player");
        }

        SPDLOG_INFO("Player plays card: color={}, type={}", card->colorToString(), card->typeToString());

        NETWORK::PlayCardPayload messagePayload = {
            {(card->getType() != GAME::CardType::WILD && card->getType() != GAME::CardType::WILDDRAWFOUR) ? card->getColor()
                                                                                                          : payload.color,
             card->getType()}};
        NETWORK::Message message = {NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::PLAY_CARD, messagePayload};
        networkClient_->send(NETWORK::MessageSerializer::serialize(message));
    }

    void UnoClient::handlePlayerDrawCard(PlayerDrawCardPayload payload)
    {
        SPDLOG_INFO("Player requests to draw card");
        NETWORK::DrawCardPayload messagePayload = {};
        NETWORK::Message message                = {NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::DRAW_CARD, messagePayload};
        networkClient_->send(NETWORK::MessageSerializer::serialize(message));
    }

    UnoClient::UnoClient()
    {
        SPDLOG_DEBUG("UnoClient initialized");
        clientGameState_ = std::make_shared<GAME::ClientGameState>();
        networkClient_   = std::make_shared<NETWORK::NetworkClient>(
            [this]() { this->handleNetworkConnected(); }, [this](const std::string &message) { this->handleNetworkMessage(message); });
        gameUI_ = std::make_shared<UI::GameUI>([this](const PlayerAction &action) { this->handlePlayerAction(action); });
    }

    UnoClient::~UnoClient()
    {
        SPDLOG_DEBUG("UnoClient shutting down");
        networkClient_->stop();
        if (networkThread_.joinable()) {
            networkThread_.join();
        }
    }

    void UnoClient::run()
    {
        SPDLOG_INFO("UnoClient starting");
        networkThread_ = std::thread([this]() { this->networkClient_->run(); });
        gameUI_->run();
    }
}   // namespace UNO::CLIENT
