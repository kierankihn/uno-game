/**
 * @file PlayerAction.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.12.07
 */

#include "PlayerAction.h"

#include <stdexcept>

namespace UNO::CLIENT {
    PlayerAction::PlayerAction(PlayerActionType type, PlayerActionPayload payload) : playerActionType(type), payload(std::move(payload))
    {
        if (playerActionType == PlayerActionType::CONNECT && std::holds_alternative<PlayerConnectPayload>(payload) == false
            || playerActionType == PlayerActionType::START_GAME && std::holds_alternative<PlayerStartGamePayload>(payload) == false
            || playerActionType == PlayerActionType::PLAY_CARD && std::holds_alternative<PlayerPlayCardPayload>(payload) == false
            || playerActionType == PlayerActionType::DRAW_CARD && std::holds_alternative<PlayerDrawCardPayload>(payload) == false) {
            throw std::invalid_argument("Payload type do not match");
        }
    }
}   // namespace UNO::CLIENT