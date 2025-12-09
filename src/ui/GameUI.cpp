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
    }

    void GameUI::doUpdateUI(const std::shared_ptr<const GAME::ClientGameState> &clientGameState)
    {
        if (clientGameState->getClientGameStage() == GAME::ClientGameStage::PENDING_CONNECTION) {
            window_->set_active_page(PageType::ConnectPage);
        }
        if (clientGameState->getClientGameStage() == GAME::ClientGameStage::PRE_GAME) {
            window_->set_active_page(PageType::StartPage);
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