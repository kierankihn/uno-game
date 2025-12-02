/**
 * @file MessageSerializer.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.20
 */
#include "MessageSerializer.h"

namespace UNO::NETWORK {
    nlohmann::json MessageSerializer::serializeCard(const GAME::Card &card)
    {
        return {{"card_color", card.colorToString()}, {"card_type", card.typeToString()}};
    }

    template<typename Iterator>
    nlohmann::json MessageSerializer::serializeCards(Iterator begin, Iterator end)
    {
        nlohmann::json res = nlohmann::json::array();
        for (auto it = begin; it != end; ++it) {
            res.push_back(serializeCard(*it));
        }
        return res;
    }

    nlohmann::json MessageSerializer::serializeDiscardPile(const GAME::DiscardPile &discardPile)
    {
        const auto &cards = discardPile.getCards();
        return serializeCards(cards.begin(), cards.end());
    }

    nlohmann::json MessageSerializer::serializePayload(const std::monostate &payload)
    {
        return nullptr;
    }

    nlohmann::json MessageSerializer::serializePayload(const JoinGamePayload &payload)
    {
        return {{"name", payload.playerName}};
    }

    nlohmann::json MessageSerializer::serializePayload(const StartGamePayload &payload)
    {
        return nullptr;
    }

    nlohmann::json MessageSerializer::serializePayload(const DrawCardPayload &payload)
    {
        return {{"draw_count", payload.drawCount}, {"cards", serializeCards(payload.cards.begin(), payload.cards.end())}};
    }

    nlohmann::json MessageSerializer::serializePayload(const PlayCardPayload &payload)
    {
        return {{"card", serializeCard(payload.card)}};
    }

    nlohmann::json MessageSerializer::serializePayload(const InitGamePayload &payload)
    {
        return {{"discard_pile", serializeDiscardPile(payload.discardPile)},
                {"hand_card", serializeCards(payload.handCard.begin(), payload.handCard.end())},
                {"current_player", payload.currentPlayerIndex}};
    }

    nlohmann::json MessageSerializer::serializePayload(const EndGamePayload &payload)
    {
        return nullptr;
    }

    std::string MessageSerializer::serializeMessagePayloadType(const MessagePayloadType &messagePayloadType)
    {
        switch (messagePayloadType) {
            case MessagePayloadType::EMPTY: return "EMPTY";
            case MessagePayloadType::JOIN_GAME: return "JOIN_GAME";
            case MessagePayloadType::START_GAME: return "START_GAME";
            case MessagePayloadType::DRAW_CARD: return "DRAW_CARD";
            case MessagePayloadType::PLAY_CARD: return "PLAY_CARD";
            case MessagePayloadType::INIT_GAME: return "INIT_GAME";
            case MessagePayloadType::END_GAME: return "END_GAME";
        }
        throw std::invalid_argument("invalid message payload type");
    }

    std::string MessageSerializer::serializeMessageStatus(const MessageStatus &messageStatus)
    {
        switch (messageStatus) {
            case MessageStatus::OK: return "OK";
            case MessageStatus::INVALID: return "INVALID";
        }
        std::unreachable();
    }

    nlohmann::json MessageSerializer::serializeMessage(const Message &message)
    {
        return {{"status_code", serializeMessageStatus(message.getMessageStatus())},
                {"payload_type", serializeMessagePayloadType(message.getMessagePayloadType())},
                {"payload", std::visit([](auto &&value) { return serializePayload(value); }, message.getMessagePayload())}};
    }

    std::string MessageSerializer::serialize(const Message &message)
    {
        return serializeMessage(message).dump();
    }

    GAME::CardColor MessageSerializer::deserializeCardColor(const std::string &cardColor)
    {
        if (cardColor == "Red") {
            return GAME::CardColor::RED;
        }
        if (cardColor == "Blue") {
            return GAME::CardColor::BLUE;
        }
        if (cardColor == "Green") {
            return GAME::CardColor::GREEN;
        }
        if (cardColor == "Yellow") {
            return GAME::CardColor::YELLOW;
        }
        throw std::invalid_argument("Invalid card color: '" + cardColor + "'. Expected: Red, Blue, Green, or Yellow");
    }

    GAME::CardType MessageSerializer::deserializeCardType(const std::string &cardType)
    {
        if (cardType == "0") {
            return GAME::CardType::NUM0;
        }
        if (cardType == "1") {
            return GAME::CardType::NUM1;
        }
        if (cardType == "2") {
            return GAME::CardType::NUM2;
        }
        if (cardType == "3") {
            return GAME::CardType::NUM3;
        }
        if (cardType == "4") {
            return GAME::CardType::NUM4;
        }
        if (cardType == "5") {
            return GAME::CardType::NUM5;
        }
        if (cardType == "6") {
            return GAME::CardType::NUM6;
        }
        if (cardType == "7") {
            return GAME::CardType::NUM7;
        }
        if (cardType == "8") {
            return GAME::CardType::NUM8;
        }
        if (cardType == "9") {
            return GAME::CardType::NUM9;
        }
        if (cardType == "Skip") {
            return GAME::CardType::SKIP;
        }
        if (cardType == "Reverse") {
            return GAME::CardType::REVERSE;
        }
        if (cardType == "Draw 2") {
            return GAME::CardType::DRAW2;
        }
        if (cardType == "Wild") {
            return GAME::CardType::WILD;
        }
        if (cardType == "Wild Draw 4") {
            return GAME::CardType::WILDDRAWFOUR;
        }
        throw std::invalid_argument("Invalid card type: '" + cardType + "'. Expected: 0-9, Skip, Reverse, Draw 2, Wild, or Wild Draw 4");
    }

    GAME::Card MessageSerializer::deserializeCard(const nlohmann::json &card)
    {
        if (card.is_object() == false) {
            throw std::invalid_argument("Invalid card format: expected JSON object");
        }
        try {
            if (card.at("card_color").is_string() == false) {
                throw std::invalid_argument("Invalid card_color field: expected string");
            }
            if (card.at("card_type").is_string() == false) {
                throw std::invalid_argument("Invalid card_type field: expected string");
            }
            return {deserializeCardColor(card.at("card_color")), deserializeCardType(card.at("card_type"))};
        }
        catch (const nlohmann::json::out_of_range &) {
            throw std::invalid_argument("Missing required field in card: expected 'card_color' and 'card_type'");
        }
    }

    GAME::DiscardPile MessageSerializer::deserializeDiscardPile(const nlohmann::json &discardPile)
    {
        if (discardPile.is_array() == false) {
            throw std::invalid_argument("Invalid discard_pile format: expected JSON array");
        }

        GAME::DiscardPile res;
        for (const auto &i : std::views::reverse(discardPile)) {
            res.add(deserializeCard(i));
        }

        return res;
    }

    std::multiset<GAME::Card> MessageSerializer::deserializeHandCard(const nlohmann::json &handCard)
    {
        if (handCard.is_array() == false) {
            throw std::invalid_argument("Invalid hand_card format: expected JSON array");
        }

        std::multiset<GAME::Card> res;
        for (const auto &i : std::views::reverse(handCard)) {
            res.insert(deserializeCard(i));
        }

        return res;
    }

    std::monostate MessageSerializer::deserializeEmptyPayload(const nlohmann::json &payload)
    {
        if (payload.is_null() == false) {
            throw std::invalid_argument("Invalid payload: expected null for empty payload");
        }
        return {};
    }


    JoinGamePayload MessageSerializer::deserializeJoinGamePayload(const nlohmann::json &payload)
    {
        try {
            if (payload.is_object() == false) {
                throw std::invalid_argument("Invalid JOIN_GAME payload: expected JSON object");
            }
            if (payload.at("name").is_string() == false) {
                throw std::invalid_argument("Invalid 'name' field in JOIN_GAME payload: expected string");
            }
            return {payload.at("name")};
        }
        catch (const nlohmann::json::out_of_range &) {
            throw std::invalid_argument("Missing required field 'name' in JOIN_GAME payload");
        }
    }

    StartGamePayload MessageSerializer::deserializeStartGamePayload(const nlohmann::json &payload)
    {
        if (payload.is_null() == false) {
            throw std::invalid_argument("Invalid START_GAME payload: expected null");
        }
        return {};
    }

    DrawCardPayload MessageSerializer::deserializeDrawCardPayload(const nlohmann::json &payload)
    {
        try {
            if (payload.is_object() == false) {
                throw std::invalid_argument("Invalid DRAW_CARD payload: expected JSON object");
            }
            if (payload.at("draw_count").is_number_unsigned() == false) {
                throw std::invalid_argument("Invalid 'draw_count' field in DRAW_CARD payload: expected unsigned integer");
            }
            if (payload.at("cards").is_array() == false) {
                throw std::invalid_argument("Invalid 'cards' field in DRAW_CARD payload: expected JSON array");
            }

            std::vector<GAME::Card> cards;
            for (const auto &i : payload.at("cards")) {
                cards.push_back(deserializeCard(i));
            }

            return {payload.at("draw_count"), cards};
        }
        catch (const nlohmann::json::out_of_range &) {
            throw std::invalid_argument("Missing required field 'draw_count' and 'cards' in DRAW_CARD payload");
        }
    }

    PlayCardPayload MessageSerializer::deserializePlayCardPayload(const nlohmann::json &payload)
    {
        try {
            if (payload.is_object() == false) {
                throw std::invalid_argument("Invalid PLAY_CARD payload: expected JSON object");
            }
            return {deserializeCard(payload.at("card"))};
        }
        catch (const nlohmann::json::out_of_range &) {
            throw std::invalid_argument("Missing required field 'card' in PLAY_CARD payload");
        }
    }

    InitGamePayload MessageSerializer::deserializeInitGamePayload(const nlohmann::json &payload)
    {
        try {
            if (payload.is_object() == false) {
                throw std::invalid_argument("Invalid INIT_GAME payload: expected JSON object");
            }
            if (payload.at("current_player").is_number_unsigned() == false) {
                throw std::invalid_argument("Invalid 'current_player' field in INIT_GAME payload: expected unsigned integer");
            }
            return {deserializeDiscardPile(payload.at("discard_pile")),
                    deserializeHandCard(payload.at("hand_card")),
                    payload.at("current_player")};
        }
        catch (const nlohmann::json::out_of_range &) {
            throw std::invalid_argument(
                "Missing required field in INIT_GAME payload: expected 'discard_pile', 'hand_card', and 'current_player'");
        }
    }

    EndGamePayload MessageSerializer::deserializeEndGamePayload(const nlohmann::json &payload)
    {
        if (payload.is_null() == false) {
            throw std::invalid_argument("Invalid END_GAME payload: expected null");
        }
        return {};
    }

    MessagePayloadType MessageSerializer::deserializeMessagePayloadType(const std::string &messagePayloadType)
    {
        if (messagePayloadType == "EMPTY") {
            return MessagePayloadType::EMPTY;
        }
        if (messagePayloadType == "JOIN_GAME") {
            return MessagePayloadType::JOIN_GAME;
        }
        if (messagePayloadType == "START_GAME") {
            return MessagePayloadType::START_GAME;
        }
        if (messagePayloadType == "DRAW_CARD") {
            return MessagePayloadType::DRAW_CARD;
        }
        if (messagePayloadType == "PLAY_CARD") {
            return MessagePayloadType::PLAY_CARD;
        }
        if (messagePayloadType == "INIT_GAME") {
            return MessagePayloadType::INIT_GAME;
        }
        if (messagePayloadType == "END_GAME") {
            return MessagePayloadType::END_GAME;
        }
        throw std::invalid_argument("Invalid message payload type: '" + messagePayloadType
                                    + "'. Expected: EMPTY, JOIN_GAME, START_GAME, DRAW_CARD, PLAY_CARD, INIT_GAME, or END_GAME");
    }

    MessageStatus MessageSerializer::deserializeMessageStatus(const std::string &messageStatus)
    {
        if (messageStatus == "OK") {
            return MessageStatus::OK;
        }
        if (messageStatus == "INVALID") {
            return MessageStatus::INVALID;
        }
        throw std::invalid_argument("Invalid message status: " + messageStatus + ". Expected: OK, INVALID");
    }

    Message MessageSerializer::deserializeMessage(const nlohmann::json &message)
    {
        try {
            if (message.is_object() == false) {
                throw std::invalid_argument("Invalid message format: expected JSON object");
            }
            if (message.at("payload_type").is_string() == false) {
                throw std::invalid_argument("Invalid 'payload_type' field: expected string");
            }
            if (message.at("status_code").is_string() == false) {
                throw std::invalid_argument("Invalid message: expected string in 'status_code'");
            }

            auto payloadType = deserializeMessagePayloadType(message.at("payload_type"));
            switch (payloadType) {
                case MessagePayloadType::EMPTY:
                    return {
                        deserializeMessageStatus(message.at("status_code")), payloadType, deserializeEmptyPayload(message.at("payload"))};
                case MessagePayloadType::JOIN_GAME:
                    return {deserializeMessageStatus(message.at("status_code")),
                            payloadType,
                            deserializeJoinGamePayload(message.at("payload"))};
                case MessagePayloadType::START_GAME:
                    return {deserializeMessageStatus(message.at("status_code")),
                            payloadType,
                            deserializeStartGamePayload(message.at("payload"))};
                case MessagePayloadType::DRAW_CARD:
                    return {deserializeMessageStatus(message.at("status_code")),
                            payloadType,
                            deserializeDrawCardPayload(message.at("payload"))};
                case MessagePayloadType::PLAY_CARD:
                    return {deserializeMessageStatus(message.at("status_code")),
                            payloadType,
                            deserializePlayCardPayload(message.at("payload"))};
                case MessagePayloadType::INIT_GAME:
                    return {deserializeMessageStatus(message.at("status_code")),
                            payloadType,
                            deserializeInitGamePayload(message.at("payload"))};
                case MessagePayloadType::END_GAME:
                    return {
                        deserializeMessageStatus(message.at("status_code")), payloadType, deserializeEndGamePayload(message.at("payload"))};
            }

            std::unreachable();
        }
        catch (const nlohmann::json::out_of_range &) {
            throw std::invalid_argument("Missing required field in message: expected 'status_code', 'payload_type' and 'payload'");
        }
    }

    Message MessageSerializer::deserialize(const std::string &data)
    {
        try {
            return deserializeMessage(nlohmann::json::parse(data));
        }
        catch (const nlohmann::json::parse_error &) {
            throw std::invalid_argument("Invalid JSON body");
        }
    }

}   // namespace UNO::NETWORK