/**
 * @file 
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */

#include "gtest/gtest.h"
#include "../../../src/game/Card.h"

TEST(card_test, card_test_1)
{
    const UNO::GAME::Card card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::NUM0);

    EXPECT_EQ(card.getColor(), UNO::GAME::CardColor::RED);
    EXPECT_EQ(card.getType(), UNO::GAME::CardType::NUM0);
    EXPECT_EQ(card.colorToString(), std::string("Red"));
    EXPECT_EQ(card.typeToString(), std::string("0"));
    EXPECT_EQ(card.toString(), std::string("Red 0"));
}

TEST(card_test, card_test_2)
{
    const UNO::GAME::Card card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::REVERSE);

    EXPECT_EQ(card.getColor(), UNO::GAME::CardColor::BLUE);
    EXPECT_EQ(card.getType(), UNO::GAME::CardType::REVERSE);
    EXPECT_EQ(card.colorToString(), std::string("Blue"));
    EXPECT_EQ(card.typeToString(), std::string("Reverse"));
    EXPECT_EQ(card.toString(), std::string("Blue Reverse"));
}

TEST(card_test, card_test_3)
{
    const UNO::GAME::Card card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::WILD);

    EXPECT_EQ(card.getColor(), UNO::GAME::CardColor::RED);
    EXPECT_EQ(card.getType(), UNO::GAME::CardType::WILD);
    EXPECT_EQ(card.colorToString(), std::string("Red"));
    EXPECT_EQ(card.typeToString(), std::string("Wild"));
    EXPECT_EQ(card.toString(), std::string("Wild"));
}

TEST(card_test, card_test_4)
{
    const UNO::GAME::Card card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::WILDDRAWFOUR);

    EXPECT_EQ(card.getColor(), UNO::GAME::CardColor::RED);
    EXPECT_EQ(card.getType(), UNO::GAME::CardType::WILDDRAWFOUR);
    EXPECT_EQ(card.colorToString(), std::string("Red"));
    EXPECT_EQ(card.typeToString(), std::string("Wild Draw 4"));
    EXPECT_EQ(card.toString(), std::string("Wild Draw 4"));
}