/**
 * @file GameState.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.16
 */
#include "GameState.h"

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

    ClientPlayerState::ClientPlayerState(std::string name, size_t remainingCardCount, bool isUno) :
        PlayerState(std::move(name), remainingCardCount, isUno)
    {
    }

    void ClientPlayerState::draw(size_t n, const std::vector<Card> &cards)
    {
        PlayerState::draw(n, cards);
    }

    Card ClientPlayerState::play(const Card &card)
    {
        return PlayerState::play(card);
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
        for (auto it = this->handCard_.getCards().begin();; it++) {
            if (it == this->handCard_.getCards().end()) {
                throw std::invalid_argument("Card not found in hand");
            }
            if (card.getType() == it->getType()
                && (card.getType() == CardType::WILD || card.getType() == CardType::WILDDRAWFOUR || card.getColor() == it->getColor())) {
                this->handCard_.play(it);
                break;
            }
        }
        return PlayerState::play(card);
    }

    bool ServerPlayerState::isEmpty() const
    {
        return this->handCard_.isEmpty();
    }

    ClientGameState::ClientGameState(std::string name) : player_(std::move(name)) {}

    const std::multiset<Card> &ClientGameState::getCards() const
    {
        return this->player_.getCards();
    }

    void ClientGameState::draw(const Card &card)
    {
        this->player_.draw(card);
    }

    void ClientGameState::draw(const std::vector<Card> &cards)
    {
        this->player_.draw(cards);
    }

    Card ClientGameState::play(const std::multiset<Card>::iterator &it)
    {
        return this->player_.play(it);
    }

    bool ClientGameState::isEmpty() const
    {
        return this->player_.isEmpty();
    }

    ServerGameState::ServerGameState() = default;

    void ServerGameState::init()
    {
        deck_.clear(), discardPile_.clear();
        while (discardPile_.isEmpty() || discardPile_.getFront().getType() > CardType::NUM9) {
            discardPile_.add(deck_.draw());
        }

        for (auto &player : this->players_) {
            while (player.isEmpty() == false) {
                player.play(*player.getCards().begin());
            }
        }

        for (size_t i = 0; i < 7; i++) {
            for (auto &player : this->players_) {
                player.draw(1, this->deck_.draw(1));
            }
        }
    }

    void ServerGameState::updateStateByDraw()
    {
        if (this->drawCount_ == 0) {
            this->drawCount_ = 1;
        }
        this->currentPlayer_->draw(this->drawCount_, deck_.draw(this->drawCount_));
        this->drawCount_ = 0;
        this->nextPlayer();
    }
}   // namespace UNO::GAME