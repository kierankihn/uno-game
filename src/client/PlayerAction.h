/**
 * @file PlayerAction.h
 *
 * @author Yuzhe Guo
 * @date 2025.12.07
 */
#pragma once
#include <string>
#include <variant>

#include "../game/Card.h"

namespace UNO::CLIENT {
    enum class PlayerActionType { CONNECT, START_GAME, PLAY_CARD, DRAW_CARD };

    struct PlayerConnectPayload {
        std::string playerName;
        std::string host;
        uint16_t port;
    };

    struct PlayerStartGamePayload {};

    struct PlayerPlayCardPayload {
        size_t id;
        GAME::CardColor color;
    };

    struct PlayerDrawCardPayload {};

    using PlayerActionPayload = std::variant<PlayerConnectPayload, PlayerStartGamePayload, PlayerPlayCardPayload, PlayerDrawCardPayload>;

    struct PlayerAction {
        PlayerActionType playerActionType;
        PlayerActionPayload payload;

        PlayerAction(PlayerActionType type, PlayerActionPayload payload);
    };
}   // namespace UNO::CLIENT
