/**
 * @file GameState.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.16
 */
#include "GameState.h"

#include <ranges>
#include <stdexcept>
#include <utility>

namespace UNO::GAME {
    PlayerState::PlayerState(std::string name, size_t remainingCardCount, bool isUno) :
        name_(std::move(name)), remainingCardCount_(remainingCardCount), isUno_(isUno)
    {
    }

    std::string PlayerState::getName() const
    {
        return this->name_;
    }

    bool PlayerState::getIsUno() const
    {
        return this->isUno_;
    }

    size_t PlayerState::getRemainingCardCount() const
    {
        return this->remainingCardCount_;
    }
    void PlayerState::setIsUno(bool x)
    {
        this->isUno_ = x;
    }

    void PlayerState::draw(size_t n, const std::vector<Card> &cards)
    {
        this->remainingCardCount_ += n;
    }

    Card PlayerState::play(const Card &card)
    {
        this->remainingCardCount_--;
        return card;
    }

    void PlayerState::clear()
    {
        this->remainingCardCount_ = 0;
    }

    ClientPlayerState::ClientPlayerState(std::string name, size_t remainingCardCount, bool isUno) :
        PlayerState(std::move(name), remainingCardCount, isUno)
    {
    }

    void ClientPlayerState::draw(size_t n, const std::vector<Card> &cards)
    {
        PlayerState::draw(n, cards);
    }

    ServerPlayerState::ServerPlayerState(std::string name, size_t remainingCardCount, bool isUno) :
        PlayerState(std::move(name), remainingCardCount, isUno)
    {
    }

    const std::multiset<Card> &ServerPlayerState::getCards() const
    {
        return this->handCard_.getCards();
    }

    void ServerPlayerState::draw(size_t x, const std::vector<Card> &cards)
    {
        PlayerState::draw(x, cards);
        this->handCard_.draw(cards);
    }


    Card ServerPlayerState::play(const Card &card)
    {
        this->handCard_.play(card);
        return PlayerState::play(card);
    }

    bool ServerPlayerState::isEmpty() const
    {
        return this->handCard_.isEmpty();
    }

    void ServerPlayerState::clear()
    {
        PlayerState::clear();
        this->handCard_.clear();
    }

    ClientGameState::ClientGameState(std::string name) : player_(std::move(name)), clientGameStage_(ClientGameStage::PENDING_CONNECTION) {}

    void ClientGameState::nextPlayer()
    {
        GameState::nextPlayer();
        if (this->self_ == this->currentPlayer_) {
            this->clientGameStage_ = ClientGameStage::ACTIVE;
        }
        else {
            this->clientGameStage_ = ClientGameStage::IDLE;
        }
    }


    const std::multiset<Card> &ClientGameState::getCards() const
    {
        return this->player_.getCards();
    }

    void ClientGameState::init(const std::vector<ClientPlayerState> &players,
                               const DiscardPile &discardPile,
                               const std::multiset<Card> &handCard,
                               const size_t &currentPlayerIndex,
                               const size_t &selfIndex)
    {
        this->players_     = players;
        this->discardPile_ = discardPile;
        player_.draw(std::vector<Card>(handCard.begin(), handCard.end()));
        this->currentPlayer_ = this->players_.begin() + static_cast<int>(currentPlayerIndex);
        this->self_          = this->players_.begin() + static_cast<int>(selfIndex);
        if (this->self_ == this->currentPlayer_) {
            this->clientGameStage_ = ClientGameStage::ACTIVE;
        }
        else {
            this->clientGameStage_ = ClientGameStage::IDLE;
        }
    }


    void ClientGameState::draw(const Card &card)
    {
        this->player_.draw(card);
    }

    void ClientGameState::draw(const std::vector<Card> &cards)
    {
        this->player_.draw(cards);
    }

    void ClientGameState::play(const Card &card)
    {
        this->player_.play(card);
    }

    bool ClientGameState::isEmpty() const
    {
        return this->player_.isEmpty();
    }

    ClientGameStage ClientGameState::getClientGameStage() const
    {
        return this->clientGameStage_;
    }

    void ClientGameState::endGame()
    {
        this->clientGameStage_ = ClientGameStage::PRE_GAME;
        this->player_.clear();
    }

    ServerGameState::ServerGameState() : serverGameStage_(ServerGameStage::PRE_GAME) {}

    void ServerGameState::init()
    {
        while (discardPile_.isEmpty() || discardPile_.getFront().getType() > CardType::NUM9) {
            discardPile_.add(deck_.draw());
        }

        for (size_t i = 0; i < 7; i++) {
            for (auto &player : this->players_) {
                player.draw(1, this->deck_.draw(1));
            }
        }

        this->serverGameStage_ = ServerGameStage::IN_GAME;
    }

    std::vector<Card> ServerGameState::updateStateByDraw()
    {
        if (this->drawCount_ == 0) {
            this->drawCount_ = 1;
        }
        auto cards = deck_.draw(this->drawCount_);
        this->currentPlayer_->draw(this->drawCount_, cards);
        this->drawCount_ = 0;
        this->nextPlayer();
        return cards;
    }

    ServerGameStage ServerGameState::getServerGameStage() const
    {
        return this->serverGameStage_;
    }

    void ServerGameState::endGame()
    {
        this->serverGameStage_ = ServerGameStage::PRE_GAME;
        deck_.clear();

        GameState::endGame();
    }
}   // namespace UNO::GAME