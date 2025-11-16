/**
 * @file PlayerTest.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.16
 */

#include "../../../src/game/Card.h"
#include "../../../src/game/Player.h"

#include <gtest/gtest.h>

TEST(player_test, player_test_1)
{
    UNO::GAME::HandCard handCard(std::array<UNO::GAME::Card, 7>{
        UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::REVERSE),
        UNO::GAME::Card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::SKIP),
        UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::NUM3),
        UNO::GAME::Card(UNO::GAME::CardColor::WILD, UNO::GAME::CardType::WILDDRAWFOUR),
        UNO::GAME::Card(UNO::GAME::CardColor::GREEN, UNO::GAME::CardType::NUM9),
        UNO::GAME::Card(UNO::GAME::CardColor::RED, UNO::GAME::CardType::NUM1),
        UNO::GAME::Card(UNO::GAME::CardColor::WILD, UNO::GAME::CardType::WILD)
    });

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