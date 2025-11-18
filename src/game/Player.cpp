/**
 * @file
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */
#include "Player.h"

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

    Card HandCard::play(const std::multiset<Card>::iterator &it)
    {
        const Card card = *it;
        cards_.erase(it);
        return card;
    }

    bool HandCard::isEmpty() const
    {
        return cards_.empty();
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

    Card Player::play(const std::multiset<Card>::iterator &it)
    {
        return this->handCard_.play(it);
    }
}   // namespace UNO::GAME
