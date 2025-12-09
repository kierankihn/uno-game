/**
 * @file GameUI.h
 *
 * @author Yuzhe Guo
 * @date 2025.12.06
 */
#pragma once
#include "../client/PlayerAction.h"
#include "../game/GameState.h"
#include "MainWindow.h"

#include <functional>
#include <memory>

namespace UNO::UI {
    class GameUI {
    private:
        std::function<void(CLIENT::PlayerAction)> callback_;
        slint::ComponentHandle<MainWindow> window_;

        void doUpdateUI(const std::shared_ptr<const GAME::ClientGameState> &clientGameState);

    public:
        explicit GameUI(std::function<void(CLIENT::PlayerAction)> callback);

        void updateUI(const std::shared_ptr<const GAME::ClientGameState> &clientGameState);

        void run();
    };

}   // namespace UNO::UI