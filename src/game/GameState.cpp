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
        PlayerState(name, remainingCardCount, isUno)
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

    template<PlayerStateTypeConcept PlayerStateType>
    GameState<PlayerStateType>::GameState(GameStatus gameStatus) :
        gameStatus_(gameStatus), isReversed_(false), drawCount_(0), players_(), currentPlayer_(players_.begin())
    {
    }

    template<PlayerStateTypeConcept PlayerStateType>
    const std::vector<PlayerStateType> &GameState<PlayerStateType>::getPlayers() const
    {
        return this->players_;
    }

    template<PlayerStateTypeConcept PlayerStateType>
    std::vector<PlayerStateType>::iterator GameState<PlayerStateType>::getCurrentPlayer() const
    {
        return this->currentPlayer_;
    }

    template<PlayerStateTypeConcept PlayerStateType>
    const DiscardPile &GameState<PlayerStateType>::getDiscardPile() const
    {
        return this->discardPile_;
    }

    template<PlayerStateTypeConcept PlayerStateType>
    bool GameState<PlayerStateType>::getIsReversed() const
    {
        return this->isReversed_;
    }

    template<PlayerStateTypeConcept PlayerStateType>
    void GameState<PlayerStateType>::addPlayer(PlayerStateType playerState)
    {
        this->currentPlayer_ = this->players_.push_back(std::move(playerState), this->currentPlayer_);
    }

    template<PlayerStateTypeConcept PlayerStateType>
    void GameState<PlayerStateType>::nextPlayer()
    {
        if (this->isReversed_ == false) {
            if (this->currentPlayer_ == this->players_.end()) {
                this->currentPlayer_ = std::next(this->currentPlayer_);
                this->currentPlayer_ = this->players_.begin();
            }
        }
        else {
            if (this->currentPlayer_ == this->players_.begin()) {
                this->currentPlayer_ = std::prev(this->players_.end());
            }
            else {
                this->currentPlayer_ = std::prev(this->currentPlayer_);
            }
        }
    }

    template<PlayerStateTypeConcept PlayerStateType>
    void GameState<PlayerStateType>::clearPlayers()
    {
        this->players_.clear();
        this->currentPlayer_ = this->players_.begin();
    }

    template<PlayerStateTypeConcept PlayerStateType>
    void GameState<PlayerStateType>::reverse()
    {
        this->isReversed_ ^= 1;
    }

    template<PlayerStateTypeConcept PlayerStateType>
    void GameState<PlayerStateType>::updateStateByCard(const Card &card)
    {
        if (this->discardPile_.isEmpty() == false && card.canBePlayedOn(this->discardPile_.getFront()) == false) {
            throw std::invalid_argument("Card cannot be played");
        }
        this->currentPlayer_->setRemainingCardCount(this->currentPlayer_->getRemainingCardCount() - 1);
        if (card.getType() == CardType::DRAW2) {
            this->drawCount_ += 2;
        }
        if (card.getType() == CardType::WILDDRAWFOUR) {
            this->drawCount_ += 4;
        }
        if (card.getType() == CardType::REVERSE) {
            this->reverse();
        }
        if (card.getType() == CardType::SKIP) {
            this->nextPlayer();
        }
        this->nextPlayer();
        this->discardPile_.add(card);
    }

    template<PlayerStateTypeConcept PlayerStateType>
    void GameState<PlayerStateType>::updateStateByDraw()
    {
        if (this->drawCount_ != 0) {
            this->drawCount_ = 0;
        }
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