/**
 * @file MessageSerializer.h
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

        template<typename Iterator>
        static nlohmann::json serializeCards(Iterator begin, Iterator end);

        static nlohmann::json serializeDiscardPile(const GAME::DiscardPile &discardPile);
        static nlohmann::json serializePlayerPublicState(const PlayerPublicState &state);
        static nlohmann::json serializePlayerPublicStates(const std::vector<PlayerPublicState> &states);

        static nlohmann::json serializePayload(const std::monostate &payload);
        static nlohmann::json serializePayload(const JoinGamePayload &payload);
        static nlohmann::json serializePayload(const StartGamePayload &payload);
        static nlohmann::json serializePayload(const PlayCardPayload &payload);
        static nlohmann::json serializePayload(const DrawCardPayload &payload);
        static nlohmann::json serializePayload(const InitGamePayload &payload);
        static nlohmann::json serializePayload(const EndGamePayload &payload);

        static std::string serializeMessagePayloadType(const MessagePayloadType &messagePayloadType);
        static std::string serializeMessageStatus(const MessageStatus &messageStatus);

        static nlohmann::json serializeMessage(const Message &message);

        static GAME::CardColor deserializeCardColor(const std::string &cardColor);
        static GAME::CardType deserializeCardType(const std::string &cardType);
        static GAME::Card deserializeCard(const nlohmann::json &card);
        static GAME::DiscardPile deserializeDiscardPile(const nlohmann::json &discardPile);
        static std::multiset<GAME::Card> deserializeHandCard(const nlohmann::json &handCard);
        static PlayerPublicState deserializePlayerPublicState(const nlohmann::json &payload);
        static std::vector<PlayerPublicState> deserializePlayerPublicStates(const nlohmann::json &payload);

        static std::monostate deserializeEmptyPayload(const nlohmann::json &payload);
        static JoinGamePayload deserializeJoinGamePayload(const nlohmann::json &payload);
        static StartGamePayload deserializeStartGamePayload(const nlohmann::json &payload);
        static PlayCardPayload deserializePlayCardPayload(const nlohmann::json &payload);
        static DrawCardPayload deserializeDrawCardPayload(const nlohmann::json &payload);
        static InitGamePayload deserializeInitGamePayload(const nlohmann::json &payload);
        static EndGamePayload deserializeEndGamePayload(const nlohmann::json &payload);

        static MessagePayloadType deserializeMessagePayloadType(const std::string &messagePayloadType);
        static MessageStatus deserializeMessageStatus(const std::string &messageStatus);

        static Message deserializeMessage(const nlohmann::json &message);
    };

}   // namespace UNO::NETWORK
