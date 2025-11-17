/**
 * @file Card.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */

#include "Card.h"
#include <format>
#include <stdexcept>

namespace UNO::GAME {
    Card::Card(const CardColor color, const CardType type) : color_(color), type_(type) {}

    CardColor Card::getColor() const
    {
        return this->color_;
    }

    CardType Card::getType() const
    {
        return this->type_;
    }

    std::string Card::colorToString() const
    {
        switch (this->type_) {
            case CardType::WILD:
            case CardType::WILDDRAWFOUR: return "Wild";
        }
        switch (this->color_) {
            case CardColor::RED: return "Red";
            case CardColor::GREEN: return "Green";
            case CardColor::BLUE: return "Blue";
            case CardColor::YELLOW: return "Yellow";
            default: throw std::invalid_argument("Invalid card color");
        }
    }

    std::string Card::typeToString() const
    {
        switch (this->type_) {
            case CardType::NUM0: return "0";
            case CardType::NUM1: return "1";
            case CardType::NUM2: return "2";
            case CardType::NUM3: return "3";
            case CardType::NUM4: return "4";
            case CardType::NUM5: return "5";
            case CardType::NUM6: return "6";
            case CardType::NUM7: return "7";
            case CardType::NUM8: return "8";
            case CardType::NUM9: return "9";
            case CardType::SKIP: return "Skip";
            case CardType::REVERSE: return "Reverse";
            case CardType::DRAW2: return "Draw 2";
            case CardType::WILD: return "Wild";
            case CardType::WILDDRAWFOUR: return "Wild Draw 4";
            default: throw std::invalid_argument("Invalid card type");
        }
    }


    std::string Card::toString() const
    {
        if (this->type_ == CardType::WILD || this->type_ == CardType::WILDDRAWFOUR) {
            return this->typeToString();
        }
        return std::format("{} {}", this->colorToString(), this->typeToString());
    }

    bool Card::operator<(const Card &other) const
    {
        if (this->color_ != other.color_) {
            return this->color_ < other.color_;
        }
        return this->type_ < other.type_;
    }

    bool Card::canBePlayedOn(const Card &other) const
    {
        if (other.getType() == CardType::DRAW2 && this->type_ != CardType::DRAW2 && this->type_ != CardType::WILDDRAWFOUR) {
            return false;
        }
        if (other.getType() == CardType::WILDDRAWFOUR && this->type_ != CardType::WILDDRAWFOUR) {
            return false;
        }
        if (this->type_ == CardType::WILD || this->type_ == CardType::WILDDRAWFOUR) {
            return true;
        }
        if (this->color_ == other.getColor() || this->type_ == other.getType()) {
            return true;
        }
        return false;
    }

}   // namespace UNO::GAME
