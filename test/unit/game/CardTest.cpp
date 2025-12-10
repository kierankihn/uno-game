/**
 * @file
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */

#include "../../../src/game/Card.h"
#include "gtest/gtest.h"

TEST(card_test, card_test_1)
{
    const UNO::GAME::Card card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::NUM0);

    EXPECT_EQ(card.getColor(), UNO::GAME::CardColor::RED);
    EXPECT_EQ(card.getType(), UNO::GAME::CardType::NUM0);
    EXPECT_EQ(card.colorToString(), std::string("red"));
    EXPECT_EQ(card.typeToString(), std::string("0"));
    EXPECT_EQ(card.toString(), std::string("red_0"));
}

TEST(card_test, card_test_2)
{
    const UNO::GAME::Card card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::REVERSE);

    EXPECT_EQ(card.getColor(), UNO::GAME::CardColor::BLUE);
    EXPECT_EQ(card.getType(), UNO::GAME::CardType::REVERSE);
    EXPECT_EQ(card.colorToString(), std::string("blue"));
    EXPECT_EQ(card.typeToString(), std::string("reverse"));
    EXPECT_EQ(card.toString(), std::string("blue_reverse"));
}

TEST(card_test, card_test_3)
{
    const UNO::GAME::Card card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::WILD);

    EXPECT_EQ(card.getColor(), UNO::GAME::CardColor::RED);
    EXPECT_EQ(card.getType(), UNO::GAME::CardType::WILD);
    EXPECT_EQ(card.colorToString(), std::string("red"));
    EXPECT_EQ(card.typeToString(), std::string("wild_wild"));
    EXPECT_EQ(card.toString(), std::string("wild_wild"));
}

TEST(card_test, card_test_4)
{
    const UNO::GAME::Card card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::WILDDRAWFOUR);

    EXPECT_EQ(card.getColor(), UNO::GAME::CardColor::RED);
    EXPECT_EQ(card.getType(), UNO::GAME::CardType::WILDDRAWFOUR);
    EXPECT_EQ(card.colorToString(), std::string("red"));
    EXPECT_EQ(card.typeToString(), std::string("wild_draw_four"));
    EXPECT_EQ(card.toString(), std::string("wild_draw_four"));
}