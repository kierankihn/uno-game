/**
 * @file GameUI.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.12.06
 */
#include "GameUI.h"
#include "MainWindow.h"

#include <utility>

#include <slint.h>

namespace UNO::UI {

    GameUI::GameUI(std::function<void(CLIENT::PlayerAction)> callback) : callback_(std::move(callback)), window_(MainWindow::create())
    {
        window_->on_request_connect([this](const auto &ui_addr, const auto &ui_port, const auto &ui_name) {
            std::string host(ui_addr);
            uint16_t port = std::stoi(std::string(ui_port));
            std::string name(ui_name);

            CLIENT::PlayerConnectPayload payload(name, host, port);
            this->callback_({CLIENT::PlayerActionType::CONNECT, payload});
            this->window_->set_is_connecting(true);
        });
        window_->on_request_start([this]() {
            CLIENT::PlayerStartGamePayload payload;
            this->callback_({CLIENT::PlayerActionType::START_GAME, payload});
            this->window_->set_is_ready(true);
        });
        window_->on_request_play_card([this](const auto &card_id, const auto &card_color) {
            GAME::CardColor game_card_color = GAME::CardColor::RED;
            switch (card_color) {
                case CardColor::Red: game_card_color = GAME::CardColor::RED; break;
                case CardColor::Green: game_card_color = GAME::CardColor::GREEN; break;
                case CardColor::Blue: game_card_color = GAME::CardColor::BLUE; break;
                case CardColor::Yellow: game_card_color = GAME::CardColor::YELLOW; break;
            }
            CLIENT::PlayerPlayCardPayload payload = {static_cast<size_t>(card_id), game_card_color};
            this->callback_({CLIENT::PlayerActionType::PLAY_CARD, payload});
        });
        window_->on_request_draw_card([this]() {
            CLIENT::PlayerDrawCardPayload payload;
            this->callback_({CLIENT::PlayerActionType::DRAW_CARD, payload});
        });

        for (auto color : GAME::AllColors) {
            for (auto type : GAME::AllTypes) {
                this->images_[{color, type}] =
                    slint::Image::load_from_path(std::format("../assets/cards/{}.svg", GAME::Card{color, type}.toString()).data());
            }
        }
    }

    void GameUI::doUpdateUI(const std::shared_ptr<const GAME::ClientGameState> &clientGameState)
    {
        if (clientGameState->getClientGameStage() == GAME::ClientGameStage::PENDING_CONNECTION) {
            window_->set_active_page(PageType::ConnectPage);
        }
        if (clientGameState->getClientGameStage() == GAME::ClientGameStage::PRE_GAME) {
            window_->set_active_page(PageType::StartPage);
        }
        if (clientGameState->getClientGameStage() == GAME::ClientGameStage::ACTIVE
            || clientGameState->getClientGameStage() == GAME::ClientGameStage::IDLE) {
            window_->set_active_page(PageType::GamePage);

            auto players = clientGameState->getPlayers();
            std::vector<OtherPlayer> other_player;
            for (auto it = players.begin(); it != players.end(); it++) {
                other_player.push_back({it->getName().data(),
                                        static_cast<int>(it->getRemainingCardCount()),
                                        it->getIsUno(),
                                        it - players.begin() == clientGameState->getCurrentPlayerId()});
            }

            auto self = other_player.begin() + static_cast<int>(clientGameState->getSelfId());
            std::ranges::rotate(other_player, self);
            other_player.pop_back();

            auto other_player_slint = std::make_shared<slint::VectorModel<OtherPlayer>>();
            other_player_slint->set_vector(other_player);

            auto cards = clientGameState->getCards();

            window_->set_other_players(other_player_slint);
            window_->set_current_player_name(clientGameState->getPlayerName().data());
            window_->set_current_player_card_count(static_cast<int>(cards.size()));
            window_->set_current_player_has_uno(false);
            window_->set_is_current_player_turn(clientGameState->getClientGameStage() == GAME::ClientGameStage::ACTIVE);

            auto hand_cards = std::make_shared<slint::VectorModel<HandCard>>();
            size_t id       = 0;
            for (auto it = cards.begin(); it != cards.end(); it++, id++) {
                hand_cards->push_back({images_[*it],
                                       false,
                                       static_cast<int>(id),
                                       it->getType() == GAME::CardType::WILD || it->getType() == GAME::CardType::WILDDRAWFOUR,
                                       it->canBePlayedOn(clientGameState->getDiscardPile().getFront(), clientGameState->getDrawCount())});
            }
            window_->set_hand_cards(hand_cards);
            window_->set_discard_top_card(images_[clientGameState->getDiscardPile().getFront()]);
            window_->set_game_direction(clientGameState->getIsReversed() ? GameDirection::Clockwise : GameDirection::CounterClockwise);

            switch (clientGameState->getDiscardPile().getFront().getColor()) {
                case GAME::CardColor::RED: window_->set_current_color(CardColor::Red); break;
                case GAME::CardColor::BLUE: window_->set_current_color(CardColor::Blue); break;
                case GAME::CardColor::GREEN: window_->set_current_color(CardColor::Green); break;
                case GAME::CardColor::YELLOW: window_->set_current_color(CardColor::Yellow); break;
            }
        }
    }

    void GameUI::updateUI(const std::shared_ptr<const GAME::ClientGameState> &clientGameState)
    {
        slint::invoke_from_event_loop([this, clientGameState]() { this->doUpdateUI(clientGameState); });
    }


    void GameUI::run()
    {
        window_->run();
    }

}   // namespace UNO::UI