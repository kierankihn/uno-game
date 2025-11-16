/**
 * @file CardTileTest.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.16
 */

#include "../../../src/game/CardTile.h"


#include <gtest/gtest.h>

TEST(card_tile_test, card_tile_test_1)
{
    const UNO::GAME::CardTile cardTile;
    ASSERT_EQ(cardTile.isEmpty(), true);
}

TEST(card_tile_test, card_tile_test_2)
{
    UNO::GAME::DiscardPile discardTile;
    ASSERT_EQ(discardTile.isEmpty(), true);
    discardTile.add(UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::NUM0));
    ASSERT_EQ(discardTile.isEmpty(), false);
}

TEST(card_tile_test, card_tile_test_3)
{
    UNO::GAME::Deck deck;
    ASSERT_EQ(deck.isEmpty(), false);
    deck.draw(108);
    ASSERT_EQ(deck.isEmpty(), true);
    deck.draw(107);
    ASSERT_EQ(deck.isEmpty(), false);
}