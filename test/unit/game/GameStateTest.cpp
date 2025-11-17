/**
 * @file GameStateTest.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.16
 */

#include "../../../src/game/GameState.h"


#include <gtest/gtest.h>


TEST(game_state_test, game_state_test_1)
{
    UNO::GAME::HandCard handCard;
    UNO::GAME::Player player(std::string("lzh"));
    player.handCard = &handCard;

    UNO::GAME::ClientGameState clientGameState(UNO::GAME::GameStatus::WAITING_PLAYERS_TO_NEXT_TURN, player);

    UNO::GAME::ClientPlayerState playerState1("pkq", 100, false);
    UNO::GAME::ClientPlayerState playerState2("kpq", 100, false);
    UNO::GAME::ClientPlayerState playerState3("qkp", 100, false);
    UNO::GAME::ClientPlayerState playerState4("lzh", 100, false);

    clientGameState.addPlayer(playerState1);
    clientGameState.addPlayer(playerState2);
    clientGameState.addPlayer(playerState3);
    clientGameState.addPlayer(playerState4);

    const auto &players = clientGameState.getPlayers();
    ASSERT_EQ(players[0].getName(), "pkq");
    ASSERT_EQ(players[1].getName(), "kpq");
    ASSERT_EQ(players[2].getName(), "qkp");
    ASSERT_EQ(players[3].getName(), "lzh");

    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "pkq");

    clientGameState.updateStateByDraw();
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "kpq");

    clientGameState.updateStateByDraw();
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "qkp");

    clientGameState.updateStateByDraw();
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "lzh");

    clientGameState.updateStateByDraw();
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "pkq");

    clientGameState.updateStateByCard(UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::REVERSE));
    ASSERT_EQ(clientGameState.getIsReversed(), true);
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "lzh");

    clientGameState.updateStateByDraw();
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "qkp");

    clientGameState.updateStateByDraw();
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "kpq");

    clientGameState.updateStateByDraw();
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "pkq");

    clientGameState.updateStateByDraw();
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "lzh");

    clientGameState.updateStateByCard(UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::SKIP));
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "kpq");

    clientGameState.updateStateByCard(UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::DRAW2));
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "pkq");
    ASSERT_EQ(clientGameState.getDrawCount(), 2);

    clientGameState.updateStateByCard(UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::DRAW2));
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "lzh");
    ASSERT_EQ(clientGameState.getDrawCount(), 4);

    clientGameState.updateStateByCard(UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::WILDDRAWFOUR));
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "qkp");
    ASSERT_EQ(clientGameState.getDrawCount(), 8);

    clientGameState.updateStateByCard(UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::WILDDRAWFOUR));
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "kpq");
    ASSERT_EQ(clientGameState.getDrawCount(), 12);

    clientGameState.updateStateByCard(UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::WILDDRAWFOUR));
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "pkq");
    ASSERT_EQ(clientGameState.getDrawCount(), 16);

    clientGameState.updateStateByCard(UNO::GAME::Card(UNO::GAME::CardColor::BLUE, UNO::GAME::CardType::WILDDRAWFOUR));
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "lzh");
    ASSERT_EQ(clientGameState.getDrawCount(), 20);

    clientGameState.updateStateByDraw();
    ASSERT_EQ(clientGameState.getPlayers()[3].getName(), "lzh");
    ASSERT_EQ(clientGameState.getPlayers()[3].getRemainingCardCount(), 120);
    ASSERT_EQ(clientGameState.getCurrentPlayer()->getName(), "qkp");
    ASSERT_EQ(clientGameState.getDrawCount(), 0);
}

TEST(game_state_test, game_state_test_2)
{
    UNO::GAME::ServerGameState serverGameState;

    serverGameState.addPlayer(UNO::GAME::ServerPlayerState("pkq1", 0, false));
    serverGameState.addPlayer(UNO::GAME::ServerPlayerState("pkq2", 0, false));
    serverGameState.addPlayer(UNO::GAME::ServerPlayerState("pkq3", 0, false));
    serverGameState.addPlayer(UNO::GAME::ServerPlayerState("lzh", 0, false));

    serverGameState.init();

    for (const auto &i : serverGameState.getPlayers()) {
        ASSERT_EQ(i.getRemainingCardCount(), 7);
    }

    while (true) {
        for (auto it = serverGameState.getCurrentPlayer()->getCards().begin();; it++) {
            const auto player = serverGameState.getCurrentPlayer();
            auto prevCards    = player->getCards();
            if (it == serverGameState.getCurrentPlayer()->getCards().end()) {
                size_t prevCount = player->getRemainingCardCount();
                size_t drawCount = serverGameState.getDrawCount();
                if (drawCount == 0) {
                    drawCount = 1;
                }

                serverGameState.updateStateByDraw();

                size_t afterCount = player->getRemainingCardCount();
                ASSERT_EQ(prevCount + drawCount, afterCount);

                for (auto card : prevCards) {
                    ASSERT_LE(prevCards.count(card), player->getCards().count(card));
                }

                break;
            }

            if (it->canBePlayedOn(serverGameState.getDiscardPile().getFront(), serverGameState.getDrawCount())) {
                auto card        = *it;
                size_t prevCount = player->getCards().count(card);

                serverGameState.updateStateByCard(card);

                size_t afterCount = player->getCards().count(card);
                ASSERT_EQ(prevCount - 1, afterCount);
                ASSERT_EQ(prevCards.size() - 1, player->getCards().size());

                for (auto i : player->getCards()) {
                    if (i.getType() != card.getType() || i.getColor() != card.getColor()) {
                        ASSERT_EQ(player->getCards().count(i), prevCards.count(i));
                    }
                }

                break;
            }
        }

        ASSERT_EQ(serverGameState.getCurrentPlayer()->getRemainingCardCount(), serverGameState.getCurrentPlayer()->getCards().size());

        if (serverGameState.getCurrentPlayer()->getRemainingCardCount() == 0) {
            break;
        }
    }
}