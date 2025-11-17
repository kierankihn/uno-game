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

    void PlayerState::setRemainingCardCount(size_t x)
    {
        this->remainingCardCount_ = x;
    }

    void PlayerState::setIsUno(bool x)
    {
        this->isUno_ = x;
    }

    ClientPlayerState::ClientPlayerState(std::string name, size_t remainingCardCount, bool isUno) :
        PlayerState(std::move(name), remainingCardCount, isUno)
    {
    }

    ServerPlayerState::ServerPlayerState(std::string name, size_t remainingCardCount, bool isUno, HandCard *handCard) :
        PlayerState(std::move(name), remainingCardCount, isUno), handCard_(handCard)
    {
    }

    template<typename... T>
    void ServerPlayerState::draw(T... args)
    {
        this->handCard_->draw(args...);
    }

    ClientGameState::ClientGameState(GameStatus gameStatus, Player player) : GameState(gameStatus), player(std::move(player)) {}

    ServerGameState::ServerGameState() : GameState(GameStatus::WAITING_PLAYERS_TO_JOIN) {}

    void ServerGameState::updateStateByDraw()
    {
        if (this->drawCount_ == 0) {
            this->drawCount_ = 1;
        }
        this->currentPlayer_->draw(this->deck_.draw(this->drawCount_));
        this->drawCount_ = 0;
    }
}   // namespace UNO::GAME