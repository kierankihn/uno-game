/**
 * @file Card.h
 * @brief 卡牌类型
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */

#ifndef UNO_GAME_CARD_H
#define UNO_GAME_CARD_H
#include <array>
#include <string>

namespace UNO::GAME {
    /**
     * @brief 卡牌颜色
     */
    enum class CardColor : uint8_t { RED, YELLOW, BLUE, GREEN, WILD };

    /**
     * 所有颜色
     */
    constexpr std::array<CardColor, 5> AllColors = {CardColor::RED, CardColor::YELLOW, CardColor::BLUE, CardColor::GREEN, CardColor::WILD};

    /**
     * @brief 卡牌类型
     */
    enum class CardType : uint8_t { NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, SKIP, REVERSE, DRAW2, WILD, WILDDRAWFOUR };

    /**
     * 所有类型
     */
    constexpr std::array<CardType, 15> AllTypes = {CardType::NUM0,
                                                   CardType::NUM1,
                                                   CardType::NUM2,
                                                   CardType::NUM3,
                                                   CardType::NUM4,
                                                   CardType::NUM5,
                                                   CardType::NUM6,
                                                   CardType::NUM7,
                                                   CardType::NUM8,
                                                   CardType::NUM9,
                                                   CardType::SKIP,
                                                   CardType::REVERSE,
                                                   CardType::DRAW2,
                                                   CardType::WILD,
                                                   CardType::WILDDRAWFOUR};

    /**
     * @brief 卡牌类
     */
    class Card {
    private:
        CardColor color_;
        CardType type_;

    public:
        Card(CardColor color, CardType value);

        /**
         * @return 卡牌的颜色
         */
        [[nodiscard]] CardColor getColor() const;

        /**
         * @return 卡牌的类型
         */
        [[nodiscard]] CardType getType() const;

        /**
         * @return 卡牌的颜色对应的字符串
         */
        [[nodiscard]] std::string colorToString() const;

        /**
         * @return 卡牌的类型对应的字符串
         */
        [[nodiscard]] std::string typeToString() const;

        /**
         * @return 卡牌对应的显示字符串
         */
        [[nodiscard]] std::string toString() const;

        /**
         * 比较运算符，用于排序
         */
        bool operator<(const Card &other) const;
    };
}   // namespace UNO::GAME

#endif   // UNO_GAME_CARD_H
