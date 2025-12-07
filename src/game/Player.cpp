/**
 * @file
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */
#include "Player.h"

#include <stdexcept>
#include <utility>

namespace UNO::GAME {
    HandCard::HandCard() = default;

    const std::multiset<Card> &HandCard::getCards() const
    {
        return cards_;
    }

    void HandCard::draw(const Card &card)
    {
        cards_.insert(card);
    }

    void HandCard::draw(const std::vector<Card> &cards)
    {
        for (const auto &card : cards) {
            cards_.insert(card);
        }
    }

    void HandCard::play(const std::multiset<Card>::iterator &it)
    {
        this->cards_.erase(it);
    }


    void HandCard::play(const Card &card)
    {
        for (auto it = cards_.begin();; it++) {
            if (it == cards_.end()) {
                throw std::invalid_argument("Card not found in hand");
            }
            if (card.getType() == it->getType()
                && (card.getType() == CardType::WILD || card.getType() == CardType::WILDDRAWFOUR || card.getColor() == it->getColor())) {
                this->play(it);
                break;
            }
        }
    }

    bool HandCard::isEmpty() const
    {
        return cards_.empty();
    }

    void HandCard::clear()
    {
        this->cards_.clear();
    }

    Player::Player(std::string name) : name_(std::move(name)) {}

    const std::string &Player::getName() const
    {
        return this->name_;
    }

    const std::multiset<Card> &Player::getCards() const
    {
        return this->handCard_.getCards();
    }

    void Player::draw(const Card &card)
    {
        this->handCard_.draw(card);
    }

    void Player::draw(const std::vector<Card> &cards)
    {
        this->handCard_.draw(cards);
    }

    bool Player::isEmpty() const
    {
        return this->handCard_.isEmpty();
    }

    void Player::play(const Card &card)
    {
        return this->handCard_.play(card);
    }

    void Player::clear()
    {
        this->handCard_.clear();
    }
}   // namespace UNO::GAME
