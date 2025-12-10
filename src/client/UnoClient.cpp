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
#include <utility>

namespace UNO::CLIENT {
    void UnoClient::handleNetworkConnected()
    {
        NETWORK::JoinGamePayload messagePayload = {this->clientGameState_->getPlayerName()};
        NETWORK::Message message                = {NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::JOIN_GAME, messagePayload};
        networkClient_->send(NETWORK::MessageSerializer::serialize(message));

        this->clientGameState_->setClientGameStageConnected();
        gameUI_->updateUI(this->clientGameState_);
    }

    void UnoClient::handleNetworkInitGame(const NETWORK::InitGamePayload &payload)
    {
        this->clientGameState_->init(payload.players, payload.discardPile, payload.handCard, payload.currentPlayerIndex, payload.playerId);
    }

    void UnoClient::handleNetworkPlayCard(const NETWORK::PlayCardPayload &payload)
    {
        if (clientGameState_->getClientGameStage() == GAME::ClientGameStage::ACTIVE) {
            clientGameState_->play(payload.card);
        }
        clientGameState_->updateStateByCard(payload.card);
    }

    void UnoClient::handleNetworkDrawCard(const NETWORK::DrawCardPayload &payload)
    {
        if (clientGameState_->getClientGameStage() == GAME::ClientGameStage::ACTIVE) {
            clientGameState_->draw(payload.cards);
        }
        clientGameState_->updateStateByDraw();
    }

    void UnoClient::handleNetworkEndGame(const NETWORK::EndGamePayload &payload)
    {
        this->clientGameState_->endGame();
    }

    void UnoClient::handleNetworkMessage(const std::string &message)
    {
        auto networkMessage = NETWORK::MessageSerializer::deserialize(message);
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
            throw std::invalid_argument("Invalid message type from server");
        }

        gameUI_->updateUI(this->clientGameState_);
    }

    void UnoClient::handlePlayerAction(PlayerAction action)
    {
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
        clientGameState_->setPlayerName(payload.playerName);
        networkClient_->connect(payload.host, payload.port);
    }


    void UnoClient::handlePlayerStartGame(PlayerStartGamePayload payload)
    {
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
            throw std::invalid_argument("Invalid card played by player");
        }

        NETWORK::PlayCardPayload messagePayload = {
            {(card->getType() != GAME::CardType::WILD && card->getType() != GAME::CardType::WILDDRAWFOUR) ? card->getColor()
                                                                                                          : payload.color,
             card->getType()}};
        NETWORK::Message message = {NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::PLAY_CARD, messagePayload};
        networkClient_->send(NETWORK::MessageSerializer::serialize(message));
    }

    void UnoClient::handlePlayerDrawCard(PlayerDrawCardPayload payload)
    {
        NETWORK::DrawCardPayload messagePayload = {};
        NETWORK::Message message                = {NETWORK::MessageStatus::OK, NETWORK::MessagePayloadType::DRAW_CARD, messagePayload};
        networkClient_->send(NETWORK::MessageSerializer::serialize(message));
    }

    UnoClient::UnoClient()
    {
        clientGameState_ = std::make_shared<GAME::ClientGameState>();
        networkClient_   = std::make_shared<NETWORK::NetworkClient>(
            [this]() { this->handleNetworkConnected(); }, [this](const std::string &message) { this->handleNetworkMessage(message); });
        gameUI_ = std::make_shared<UI::GameUI>([this](const PlayerAction &action) { this->handlePlayerAction(action); });
    }

    void UnoClient::run()
    {
        networkThread_ = std::thread([this]() { this->networkClient_->run(); });
        gameUI_->run();
    }

}   // namespace UNO::CLIENT