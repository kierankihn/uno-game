/**
 * @file CardTile.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */
#include "CardTile.h"
#include <algorithm>
#include <random>

#include "../common/Utils.h"

namespace UNO::GAME {
    CardTile::CardTile() = default;

    template<typename... T>
    void CardTile::pushFront(T... t)
    {
        cards_.emplace_front(t...);
    }

    template<typename... T>
    void CardTile::pushBack(T... t)
    {
        cards_.emplace_back(t...);
    }

    Card CardTile::popFront()
    {
        const Card card = cards_.front();
        cards_.pop_front();
        return card;
    }

    Card CardTile::popBack()
    {
        const Card card = cards_.back();
        cards_.pop_back();
        return card;
    }

    Card CardTile::front() const
    {
        return cards_.front();
    }

    bool CardTile::isEmpty() const
    {
        return cards_.empty();
    }

    void CardTile::shuffle()
    {
        std::ranges::shuffle(cards_, COMMON::Utils::getInstance()->getRandom().getGenerator());
    }

    void CardTile::clear()
    {
        cards_.clear();
    }

    DiscardPile::DiscardPile() = default;

    void DiscardPile::add(Card card)
    {
        this->pushFront(card);
    }

    Card DiscardPile::getFront() const
    {
        return this->front();
    }

    Deck::Deck()
    {
        this->init();
    }

    void Deck::init()
    {
        for (const auto color : AllColors) {
            for (const auto type : AllTypes) {
                if (color != CardColor::WILD && type != CardType::WILD && type != CardType::WILDDRAWFOUR) {
                    for (int i = 1 + (type == CardType::NUM0); i <= 2; i++) {
                        this->pushBack(color, type);
                    }
                }
            }
        }
        for (int i = 0; i < 4; i++) {
            this->pushBack(CardColor::WILD, CardType::WILD);
            this->pushBack(CardColor::WILD, CardType::WILDDRAWFOUR);
        }
        this->shuffle();
    }

    Card Deck::draw()
    {
        if (this->isEmpty()) {
            this->init();
        }
        return this->popFront();
    }

    std::vector<Card> Deck::draw(const size_t n)
    {
        std::vector<Card> draw_card;
        for (size_t i = 0; i < n; i++) {
            draw_card.push_back(this->draw());
        }
        return draw_card;
    }
}   // namespace UNO::GAME
