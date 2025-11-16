/**
 * @file Player.h
 *
 * 玩家
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */
#ifndef UNO_GAME_PLAYER_H
#define UNO_GAME_PLAYER_H
#include <array>
#include <set>
#include <vector>

#include "Card.h"

namespace UNO::GAME {
    /**
     * 玩家手牌
     */
    class HandCard {
    private:
        std::multiset<Card> cards_;

    public:
        explicit HandCard(const std::array<Card, 7> &cards);

        /**
         * 获得当前手牌
         * @return 当前手牌的集合
         */
        [[nodiscard]] const std::multiset<Card> &getCards() const;

        /**
         * 摸一张牌
         * @param card 摸的牌
         */
        void draw(const Card &card);

        /**
         * 摸多张牌
         * @param cards 摸的牌
         */
        void draw(const std::vector<Card> &cards);

        /**
         * 打出一张牌
         * @param it 要打出的手牌的迭代器
         * @return 打出的手牌
         */
        Card play(const std::multiset<Card>::iterator &it);

        /**
         * @return 手牌是否为空
         */
        [[nodiscard]] bool isEmpty() const;
    };

    /**
     * 玩家
     */
    class Player {
    private:
        std::string name_;

    public:
        HandCard *handCard;

        explicit Player(std::string &name);

        /**
         * @return 返回玩家名字
         */
        [[nodiscard]] const std::string &getName() const;
    };
}   // namespace UNO::GAME

#endif   // UNO_GAME_PLAYER_H
