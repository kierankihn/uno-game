/**
 * @file
 *
 * @author Yuzhe Guo
 * @date 2025.11.20
 */
#pragma once
#include "Message.h"

#include <nlohmann/json.hpp>
#include <string>

namespace UNO::NETWORK {

    class MessageSerializer {
    public:
        static std::string serialize(const Message &message);
        static Message deserialize(const std::string &data);

    private:
        static nlohmann::json serializeCard(const GAME::Card &card);
        static nlohmann::json serializeDiscardPile(const GAME::DiscardPile &discardPile);
        static nlohmann::json serializeHandCard(const GAME::HandCard &handCard);

        static nlohmann::json serializePayload(const std::monostate &payload);
        static nlohmann::json serializePayload(const JoinGamePayload &payload);
        static nlohmann::json serializePayload(const StartGamePayload &payload);
        static nlohmann::json serializePayload(const PlayCardPayload &payload);
        static nlohmann::json serializePayload(const DrawCardPayload &payload);
        static nlohmann::json serializePayload(const InitGamePayload &payload);
        static nlohmann::json serializePayload(const EndGamePayload &payload);

        static std::string serializeMessagePayloadType(const MessagePayloadType &messagePayloadType);

        static nlohmann::json serializeMessage(const Message &message);

        static GAME::CardColor deserializeCardColor(const std::string &cardColor);
        static GAME::CardType deserializeCardType(const std::string &cardType);
        static GAME::Card deserializeCard(const nlohmann::json &card);
        static GAME::DiscardPile deserializeDiscardPile(const nlohmann::json &discardPile);
        static GAME::HandCard deserializeHandCard(const nlohmann::json &handCard);

        static std::monostate deserializeMonostatePayload(const nlohmann::json &payload);
        static JoinGamePayload deserializeJoinGamePayload(const nlohmann::json &payload);
        static StartGamePayload deserializeStartGamePayload(const nlohmann::json &payload);
        static PlayCardPayload deserializePlayCardPayload(const nlohmann::json &payload);
        static DrawCardPayload deserializeDrawCardPayload(const nlohmann::json &payload);
        static InitGamePayload deserializeInitGamePayload(const nlohmann::json &payload);
        static EndGamePayload deserializeEndGamePayload(const nlohmann::json &payload);

        static MessagePayloadType deserializeMessagePayloadType(const std::string &messagePayloadType);
        static Message deserializeMessage(const nlohmann::json &message);
    };

}   // namespace UNO::NETWORK