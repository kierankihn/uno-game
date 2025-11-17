/**
 * @file PlayerTest.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.16
 */

#include "../../../src/game/Player.h"
#include "../../../src/game/Card.h"

#include <gtest/gtest.h>

TEST(player_test, player_test_1)
{
    UNO::GAME::HandCard handCard;

    handCard.draw(UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::REVERSE));
    handCard.draw(UNO::GAME::Card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::SKIP));
    handCard.draw(UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::NUM3));
    handCard.draw(UNO::GAME::Card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::WILDDRAWFOUR));
    handCard.draw(UNO::GAME::Card(UNO::GAME::CardColor::GREEN, UNO::GAME::CardType::NUM9));
    handCard.draw(UNO::GAME::Card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::NUM1));
    handCard.draw(UNO::GAME::Card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::WILD));

    ASSERT_EQ(handCard.getCards().begin()->getColor(), UNO::GAME::CardColor::RED);
    ASSERT_EQ(handCard.getCards().begin()->getType(), UNO::GAME::CardType::NUM1);

    handCard.play(handCard.getCards().begin());

    ASSERT_EQ(handCard.getCards().begin()->getColor(), UNO::GAME::CardColor::RED);
    ASSERT_EQ(handCard.getCards().begin()->getType(), UNO::GAME::CardType::SKIP);

    handCard.draw(UNO::GAME::Card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::NUM0));

    ASSERT_EQ(handCard.getCards().begin()->getColor(), UNO::GAME::CardColor::RED);
    ASSERT_EQ(handCard.getCards().begin()->getType(), UNO::GAME::CardType::NUM0);

    ASSERT_EQ(handCard.isEmpty(), false);
}