/**
 * @file UnoClient.h
 *
 * @author Yuzhe Guo
 * @date 2025.12.06
 */
#pragma once
#include "../game/GameState.h"
#include "../network/Message.h"
#include "../network/NetworkClient.h"
#include "../ui/GameUI.h"
#include "PlayerAction.h"

#include <memory>
#include <thread>


namespace UNO::CLIENT {
    class UnoClient {
    private:
        std::shared_ptr<GAME::ClientGameState> clientGameState_;
        std::shared_ptr<UI::GameUI> gameUI_;
        std::shared_ptr<NETWORK::NetworkClient> networkClient_;

        std::thread networkThread_;

    private:
        void handleNetworkConnected();

        void handleNetworkMessage(const std::string &message);

        void handleNetworkInitGame(const NETWORK::InitGamePayload &payload);
        void handleNetworkPlayCard(const NETWORK::PlayCardPayload &payload);
        void handleNetworkDrawCard(const NETWORK::DrawCardPayload &payload);
        void handleNetworkEndGame(const NETWORK::EndGamePayload &payload);


        void handlePlayerAction(PlayerAction action);

        void handlePlayerConnect(const PlayerConnectPayload &payload);
        void handlePlayerStartGame(PlayerStartGamePayload payload);
        void handlePlayerPlayCard(PlayerPlayCardPayload payload);
        void handlePlayerDrawCard(PlayerDrawCardPayload payload);

    public:
        UnoClient();

        void run();
    };

}   // namespace UNO::CLIENT