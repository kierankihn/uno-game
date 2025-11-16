/**
 * @file CardTile.h
 *
 * 牌堆
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */
#ifndef UNO_GAME_CARDTILE_H
#define UNO_GAME_CARDTILE_H
#include <deque>
#include <vector>

#include "Card.h"

namespace UNO::GAME {
    /**
     * @brief 牌堆
     */
    class CardTile {
    private:
        std::deque<Card> cards_;

    protected:
        /**
         * 向牌堆前加入卡牌
         * @param t 要加入的卡牌
         */
        template<typename... T>
        void pushFront(T... t);

        /**
        * 向牌堆后加入卡牌
        * @param t 要加入的卡牌
        */
        template<typename... T>
        void pushBack(T... t);

        /**
        * 将牌堆中第一张牌删除
        * @return 删除的牌
        */
        Card popFront();

        /**
        * 将牌堆中最后一张牌删除
        * @return 删除的牌
        */
        Card popBack();

        /**
         * @return 牌堆中最上方的牌
         */
        [[nodiscard]] Card front() const;

        /**
         * 将牌堆中的卡牌随机洗混
         */
        void shuffle();

        /**
         * 清空牌堆
         */
        void clear();

    public:
        CardTile();

        /**
         * @return 牌堆是否为空
         */
        [[nodiscard]] bool isEmpty() const;
    };

    /**
     * @brief 弃牌堆
     */
    class DiscardPile : public CardTile {
    public:
        DiscardPile();

        /**
         * 向弃牌堆中添加 @param card 卡牌
         */
        void add(Card card);

        /**
         * @return 牌堆中最上方的牌
         */
        Card getFront() const;
    };

    /**
     * @brief 起牌堆
     */
    class Deck : public CardTile {
    public:
        Deck();

        /**
         * 初始化牌堆
         */
        void init();

        /**
         * 摸一张牌
         * @return 摸到的牌
         */
        Card draw();

        /**
         * 摸 n 张牌
         * @param n 摸牌的张数
         * @return 摸到的牌
         */
        std::vector<Card> draw(size_t n);
    };
}

#endif //UNO_GAME_CARDTILE_H
