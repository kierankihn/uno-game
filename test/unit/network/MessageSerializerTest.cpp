/**
 * @file MessageSerializerTest.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.21
 */

#include "../../../src/network/MessageSerializer.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace UNO::NETWORK;
using namespace UNO::GAME;

// ========== Serialize Tests ==========

TEST(MessageSerializerTest, SerializeJoinGameMessage)
{
    JoinGamePayload payload{"Player1"};
    Message message(MessageStatus::OK, MessagePayloadType::JOIN_GAME, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
    EXPECT_EQ(json["payload_type"], "JOIN_GAME");
    EXPECT_EQ(json["payload"]["name"], "Player1");
}

TEST(MessageSerializerTest, SerializeJoinGameMessageWithEmptyName)
{
    JoinGamePayload payload{""};
    Message message(MessageStatus::OK, MessagePayloadType::JOIN_GAME, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
    EXPECT_EQ(json["payload_type"], "JOIN_GAME");
    EXPECT_EQ(json["payload"]["name"], "");
}

TEST(MessageSerializerTest, SerializeJoinGameMessageWithSpecialCharacters)
{
    JoinGamePayload payload{"Player<>\"&\n\t"};
    Message message(MessageStatus::OK, MessagePayloadType::JOIN_GAME, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
    EXPECT_EQ(json["payload_type"], "JOIN_GAME");
    EXPECT_EQ(json["payload"]["name"], "Player<>\"&\n\t");
}

TEST(MessageSerializerTest, SerializeStartGameMessage)
{
    StartGamePayload payload{};
    Message message(MessageStatus::OK, MessagePayloadType::START_GAME, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
    EXPECT_EQ(json["payload_type"], "START_GAME");
    EXPECT_TRUE(json["payload"].is_null());
}

TEST(MessageSerializerTest, SerializeDrawCardMessage)
{
    std::vector<Card> cards = {Card(CardColor::RED, CardType::NUM5), Card(CardColor::BLUE, CardType::SKIP)};
    DrawCardPayload payload{5, cards};
    Message message(MessageStatus::OK, MessagePayloadType::DRAW_CARD, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
    EXPECT_EQ(json["payload_type"], "DRAW_CARD");
    EXPECT_EQ(json["payload"]["draw_count"], 5);
    EXPECT_TRUE(json["payload"]["cards"].is_array());
    EXPECT_EQ(json["payload"]["cards"].size(), 2);
    EXPECT_EQ(json["payload"]["cards"][0]["card_color"], "Red");
    EXPECT_EQ(json["payload"]["cards"][0]["card_type"], "5");
    EXPECT_EQ(json["payload"]["cards"][1]["card_color"], "Blue");
    EXPECT_EQ(json["payload"]["cards"][1]["card_type"], "Skip");
}

TEST(MessageSerializerTest, SerializeDrawCardMessageWithZero)
{
    DrawCardPayload payload{0, {}};
    Message message(MessageStatus::OK, MessagePayloadType::DRAW_CARD, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
    EXPECT_EQ(json["payload_type"], "DRAW_CARD");
    EXPECT_EQ(json["payload"]["draw_count"], 0);
    EXPECT_TRUE(json["payload"]["cards"].is_array());
    EXPECT_EQ(json["payload"]["cards"].size(), 0);
}

TEST(MessageSerializerTest, SerializeDrawCardMessageWithMultipleCards)
{
    std::vector<Card> cards = {Card(CardColor::RED, CardType::NUM1),
                               Card(CardColor::GREEN, CardType::NUM2),
                               Card(CardColor::YELLOW, CardType::REVERSE),
                               Card(CardColor::BLUE, CardType::DRAW2)};
    DrawCardPayload payload{4, cards};
    Message message(MessageStatus::OK, MessagePayloadType::DRAW_CARD, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
    EXPECT_EQ(json["payload_type"], "DRAW_CARD");
    EXPECT_EQ(json["payload"]["draw_count"], 4);
    EXPECT_TRUE(json["payload"]["cards"].is_array());
    EXPECT_EQ(json["payload"]["cards"].size(), 4);
}

TEST(MessageSerializerTest, SerializePlayCardMessage)
{
    Card card(CardColor::RED, CardType::NUM5);
    PlayCardPayload payload{card};
    Message message(MessageStatus::OK, MessagePayloadType::PLAY_CARD, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
    EXPECT_EQ(json["payload_type"], "PLAY_CARD");
    EXPECT_EQ(json["payload"]["card"]["card_color"], "Red");
    EXPECT_EQ(json["payload"]["card"]["card_type"], "5");
}

TEST(MessageSerializerTest, SerializePlayCardMessageAllColors)
{
    std::vector<std::pair<CardColor, std::string>> colors = {
        {CardColor::RED, "Red"}, {CardColor::BLUE, "Blue"}, {CardColor::GREEN, "Green"}, {CardColor::YELLOW, "Yellow"}};

    for (const auto &[color, expected] : colors) {
        Card card(color, CardType::NUM1);
        PlayCardPayload payload{card};
        Message message(MessageStatus::OK, MessagePayloadType::PLAY_CARD, payload);

        std::string result  = MessageSerializer::serialize(message);
        nlohmann::json json = nlohmann::json::parse(result);

        EXPECT_EQ(json["status_code"], "OK");
        EXPECT_EQ(json["payload"]["card"]["card_color"], expected);
    }
}

TEST(MessageSerializerTest, SerializePlayCardMessageAllTypes)
{
    std::vector<std::pair<CardType, std::string>> types = {{CardType::NUM0, "0"},
                                                           {CardType::NUM1, "1"},
                                                           {CardType::NUM2, "2"},
                                                           {CardType::NUM3, "3"},
                                                           {CardType::NUM4, "4"},
                                                           {CardType::NUM5, "5"},
                                                           {CardType::NUM6, "6"},
                                                           {CardType::NUM7, "7"},
                                                           {CardType::NUM8, "8"},
                                                           {CardType::NUM9, "9"},
                                                           {CardType::SKIP, "Skip"},
                                                           {CardType::REVERSE, "Reverse"},
                                                           {CardType::DRAW2, "Draw 2"},
                                                           {CardType::WILD, "Wild"},
                                                           {CardType::WILDDRAWFOUR, "Wild Draw 4"}};

    for (const auto &[type, expected] : types) {
        Card card(CardColor::RED, type);
        PlayCardPayload payload{card};
        Message message(MessageStatus::OK, MessagePayloadType::PLAY_CARD, payload);

        std::string result  = MessageSerializer::serialize(message);
        nlohmann::json json = nlohmann::json::parse(result);

        EXPECT_EQ(json["status_code"], "OK");
        EXPECT_EQ(json["payload"]["card"]["card_type"], expected);
    }
}

TEST(MessageSerializerTest, SerializeInitGameMessage)
{
    DiscardPile discardPile;
    discardPile.add(Card(CardColor::RED, CardType::NUM5));
    discardPile.add(Card(CardColor::BLUE, CardType::SKIP));

    HandCard handCard;
    handCard.draw(Card(CardColor::GREEN, CardType::NUM3));
    handCard.draw(Card(CardColor::YELLOW, CardType::REVERSE));

    InitGamePayload payload{discardPile, handCard, 2};
    Message message(MessageStatus::OK, MessagePayloadType::INIT_GAME, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
    EXPECT_EQ(json["payload_type"], "INIT_GAME");
    EXPECT_EQ(json["payload"]["current_player"], 2);
    EXPECT_TRUE(json["payload"]["discard_pile"].is_array());
    EXPECT_TRUE(json["payload"]["hand_card"].is_array());
    EXPECT_EQ(json["payload"]["discard_pile"].size(), 2);
    EXPECT_EQ(json["payload"]["hand_card"].size(), 2);
}

TEST(MessageSerializerTest, SerializeInitGameMessageWithEmptyPiles)
{
    DiscardPile discardPile;
    HandCard handCard;

    InitGamePayload payload{discardPile, handCard, 0};
    Message message(MessageStatus::OK, MessagePayloadType::INIT_GAME, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
    EXPECT_EQ(json["payload_type"], "INIT_GAME");
    EXPECT_EQ(json["payload"]["current_player"], 0);
    EXPECT_TRUE(json["payload"]["discard_pile"].is_array());
    EXPECT_TRUE(json["payload"]["hand_card"].is_array());
    EXPECT_EQ(json["payload"]["discard_pile"].size(), 0);
    EXPECT_EQ(json["payload"]["hand_card"].size(), 0);
}

TEST(MessageSerializerTest, SerializeEndGameMessage)
{
    EndGamePayload payload{};
    Message message(MessageStatus::OK, MessagePayloadType::END_GAME, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
    EXPECT_EQ(json["payload_type"], "END_GAME");
    EXPECT_TRUE(json["payload"].is_null());
}

// ========== Serialize Status Tests ==========

TEST(MessageSerializerTest, SerializeMessageWithOKStatus)
{
    JoinGamePayload payload{"TestPlayer"};
    Message message(MessageStatus::OK, MessagePayloadType::JOIN_GAME, payload);

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "OK");
}

TEST(MessageSerializerTest, SerializeMessageWithINVALIDStatus)
{
    Message message(MessageStatus::INVALID, MessagePayloadType::EMPTY, std::monostate{});

    std::string result  = MessageSerializer::serialize(message);
    nlohmann::json json = nlohmann::json::parse(result);

    EXPECT_EQ(json["status_code"], "INVALID");
    EXPECT_EQ(json["payload_type"], "EMPTY");
    EXPECT_TRUE(json["payload"].is_null());
}

// ========== Deserialize Status Tests ==========

TEST(MessageSerializerTest, DeserializeMessageWithOKStatus)
{
    std::string json = R"({"status_code":"OK","payload_type":"START_GAME","payload":null})";

    Message message = MessageSerializer::deserialize(json);

    EXPECT_EQ(message.getMessageStatus(), MessageStatus::OK);
}

TEST(MessageSerializerTest, DeserializeMessageWithINVALIDStatus)
{
    std::string json = R"({"status_code":"INVALID","payload_type":"EMPTY","payload":null})";

    Message message = MessageSerializer::deserialize(json);

    EXPECT_EQ(message.getMessageStatus(), MessageStatus::INVALID);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::EMPTY);
}

TEST(MessageSerializerTest, RoundTripMessageWithINVALIDStatus)
{
    Message original(MessageStatus::INVALID, MessagePayloadType::EMPTY, std::monostate{});

    std::string serialized = MessageSerializer::serialize(original);
    Message deserialized   = MessageSerializer::deserialize(serialized);

    EXPECT_EQ(deserialized.getMessageStatus(), MessageStatus::INVALID);
    EXPECT_EQ(deserialized.getMessagePayloadType(), MessagePayloadType::EMPTY);
}

// ========== Deserialize Tests ==========

TEST(MessageSerializerTest, DeserializeJoinGameMessage)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":"Player1"}})";

    Message message = MessageSerializer::deserialize(json);

    EXPECT_EQ(message.getMessageStatus(), MessageStatus::OK);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::JOIN_GAME);
    auto payload = std::get<JoinGamePayload>(message.getMessagePayload());
    EXPECT_EQ(payload.playerName, "Player1");
}

TEST(MessageSerializerTest, DeserializeJoinGameMessageWithEmptyName)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":""}})";

    Message message = MessageSerializer::deserialize(json);

    EXPECT_EQ(message.getMessageStatus(), MessageStatus::OK);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::JOIN_GAME);
    auto payload = std::get<JoinGamePayload>(message.getMessagePayload());
    EXPECT_EQ(payload.playerName, "");
}

TEST(MessageSerializerTest, DeserializeStartGameMessage)
{
    std::string json = R"({"status_code":"OK","payload_type":"START_GAME","payload":null})";

    Message message = MessageSerializer::deserialize(json);

    EXPECT_EQ(message.getMessageStatus(), MessageStatus::OK);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::START_GAME);
}

TEST(MessageSerializerTest, DeserializeDrawCardMessage)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":5,"cards":[{"card_color":"Red","card_type":"5"},{"card_color":"Blue","card_type":"Skip"}]}})";

    Message message = MessageSerializer::deserialize(json);

    EXPECT_EQ(message.getMessageStatus(), MessageStatus::OK);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::DRAW_CARD);
    auto payload = std::get<DrawCardPayload>(message.getMessagePayload());
    EXPECT_EQ(payload.drawCount, 5);
    EXPECT_EQ(payload.cards.size(), 2);
    EXPECT_EQ(payload.cards[0].getColor(), CardColor::RED);
    EXPECT_EQ(payload.cards[0].getType(), CardType::NUM5);
    EXPECT_EQ(payload.cards[1].getColor(), CardColor::BLUE);
    EXPECT_EQ(payload.cards[1].getType(), CardType::SKIP);
}

TEST(MessageSerializerTest, DeserializeDrawCardMessageWithEmptyCards)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":0,"cards":[]}})";

    Message message = MessageSerializer::deserialize(json);

    EXPECT_EQ(message.getMessageStatus(), MessageStatus::OK);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::DRAW_CARD);
    auto payload = std::get<DrawCardPayload>(message.getMessagePayload());
    EXPECT_EQ(payload.drawCount, 0);
    EXPECT_EQ(payload.cards.size(), 0);
}

TEST(MessageSerializerTest, DeserializePlayCardMessage)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":"Red","card_type":"5"}}})";

    Message message = MessageSerializer::deserialize(json);

    EXPECT_EQ(message.getMessageStatus(), MessageStatus::OK);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::PLAY_CARD);
    auto payload = std::get<PlayCardPayload>(message.getMessagePayload());
    EXPECT_EQ(payload.card.getColor(), CardColor::RED);
    EXPECT_EQ(payload.card.getType(), CardType::NUM5);
}

TEST(MessageSerializerTest, DeserializeInitGameMessage)
{
    std::string json = R"({
        "status_code":"OK",
        "payload_type":"INIT_GAME",
        "payload":{
            "discard_pile":[{"card_color":"Red","card_type":"5"}],
            "hand_card":[{"card_color":"Blue","card_type":"Skip"}],
            "current_player":2
        }
    })";

    Message message = MessageSerializer::deserialize(json);

    EXPECT_EQ(message.getMessageStatus(), MessageStatus::OK);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::INIT_GAME);
    auto payload = std::get<InitGamePayload>(message.getMessagePayload());
    EXPECT_EQ(payload.currentPlayerIndex, 2);
}

TEST(MessageSerializerTest, DeserializeEndGameMessage)
{
    std::string json = R"({"status_code":"OK","payload_type":"END_GAME","payload":null})";

    Message message = MessageSerializer::deserialize(json);

    EXPECT_EQ(message.getMessageStatus(), MessageStatus::OK);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::END_GAME);
}

// ========== Round-trip Tests ==========

TEST(MessageSerializerTest, RoundTripJoinGameMessage)
{
    JoinGamePayload payload{"Player1"};
    Message original(MessageStatus::OK, MessagePayloadType::JOIN_GAME, payload);

    std::string serialized = MessageSerializer::serialize(original);
    Message deserialized   = MessageSerializer::deserialize(serialized);

    EXPECT_EQ(deserialized.getMessageStatus(), MessageStatus::OK);
    EXPECT_EQ(deserialized.getMessagePayloadType(), MessagePayloadType::JOIN_GAME);
    auto deserializedPayload = std::get<JoinGamePayload>(deserialized.getMessagePayload());
    EXPECT_EQ(deserializedPayload.playerName, "Player1");
}

TEST(MessageSerializerTest, RoundTripAllMessageTypes)
{
    std::vector<Message> messages;

    messages.emplace_back(MessageStatus::OK, MessagePayloadType::JOIN_GAME, JoinGamePayload{"TestPlayer"});
    messages.emplace_back(MessageStatus::OK, MessagePayloadType::START_GAME, StartGamePayload{});
    messages.emplace_back(MessageStatus::OK,
                          MessagePayloadType::DRAW_CARD,
                          DrawCardPayload{3, {Card(CardColor::RED, CardType::NUM1), Card(CardColor::BLUE, CardType::NUM2)}});
    messages.emplace_back(MessageStatus::OK, MessagePayloadType::PLAY_CARD, PlayCardPayload{Card(CardColor::RED, CardType::NUM5)});
    messages.emplace_back(MessageStatus::OK, MessagePayloadType::END_GAME, EndGamePayload{});

    for (const auto &original : messages) {
        std::string serialized = MessageSerializer::serialize(original);
        Message deserialized   = MessageSerializer::deserialize(serialized);
        EXPECT_EQ(deserialized.getMessageStatus(), original.getMessageStatus());
        EXPECT_EQ(deserialized.getMessagePayloadType(), original.getMessagePayloadType());
    }
}

TEST(MessageSerializerTest, RoundTripDrawCardMessageWithCards)
{
    std::vector<Card> cards = {
        Card(CardColor::GREEN, CardType::NUM3), Card(CardColor::YELLOW, CardType::REVERSE), Card(CardColor::RED, CardType::WILD)};
    DrawCardPayload payload{3, cards};
    Message original(MessageStatus::OK, MessagePayloadType::DRAW_CARD, payload);

    std::string serialized = MessageSerializer::serialize(original);
    Message deserialized   = MessageSerializer::deserialize(serialized);

    EXPECT_EQ(deserialized.getMessageStatus(), MessageStatus::OK);
    EXPECT_EQ(deserialized.getMessagePayloadType(), MessagePayloadType::DRAW_CARD);
    auto deserializedPayload = std::get<DrawCardPayload>(deserialized.getMessagePayload());
    EXPECT_EQ(deserializedPayload.drawCount, 3);
    EXPECT_EQ(deserializedPayload.cards.size(), 3);
    EXPECT_EQ(deserializedPayload.cards[0].getColor(), CardColor::GREEN);
    EXPECT_EQ(deserializedPayload.cards[0].getType(), CardType::NUM3);
    EXPECT_EQ(deserializedPayload.cards[1].getColor(), CardColor::YELLOW);
    EXPECT_EQ(deserializedPayload.cards[1].getType(), CardType::REVERSE);
    EXPECT_EQ(deserializedPayload.cards[2].getColor(), CardColor::RED);
    EXPECT_EQ(deserializedPayload.cards[2].getType(), CardType::WILD);
}

// ========== Invalid Input Tests ==========

TEST(MessageSerializerTest, DeserializeInvalidJSON)
{
    std::string json = "invalid json";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeNonObjectJSON)
{
    std::string json = R"([1,2,3])";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeNullJSON)
{
    std::string json = "null";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeStringJSON)
{
    std::string json = R"("string")";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeNumberJSON)
{
    std::string json = "123";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeBooleanJSON)
{
    std::string json = "true";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeMissingPayloadType)
{
    std::string json = R"({"status_code":"OK","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeMissingStatusCode)
{
    std::string json = R"({"payload_type":"START_GAME","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeMissingPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"START_GAME"})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInvalidPayloadType)
{
    std::string json = R"({"status_code":"OK","payload_type":"INVALID_TYPE","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInvalidStatusCode)
{
    std::string json = R"({"status_code":"UNKNOWN","payload_type":"START_GAME","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePayloadTypeAsNumber)
{
    std::string json = R"({"status_code":"OK","payload_type":123,"payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeStatusCodeAsNumber)
{
    std::string json = R"({"status_code":200,"payload_type":"START_GAME","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePayloadTypeAsNull)
{
    std::string json = R"({"status_code":"OK","payload_type":null,"payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeStatusCodeAsNull)
{
    std::string json = R"({"status_code":null,"payload_type":"START_GAME","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePayloadTypeAsArray)
{
    std::string json = R"({"status_code":"OK","payload_type":[],"payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeStatusCodeAsArray)
{
    std::string json = R"({"status_code":[],"payload_type":"START_GAME","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePayloadTypeAsObject)
{
    std::string json = R"({"status_code":"OK","payload_type":{},"payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeStatusCodeAsObject)
{
    std::string json = R"({"status_code":{},"payload_type":"START_GAME","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

// ========== JoinGamePayload Invalid Tests ==========

TEST(MessageSerializerTest, DeserializeJoinGameWithNullPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeJoinGameWithArrayPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":[]})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeJoinGameWithNumberPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":123})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeJoinGameWithMissingName)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeJoinGameWithNumberName)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":123}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeJoinGameWithNullName)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":null}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeJoinGameWithArrayName)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":[]}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeJoinGameWithObjectName)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":{}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

// ========== StartGamePayload Invalid Tests ==========

TEST(MessageSerializerTest, DeserializeStartGameWithNonNullPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"START_GAME","payload":{}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeStartGameWithArrayPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"START_GAME","payload":[]})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeStartGameWithStringPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"START_GAME","payload":"test"})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

// ========== DrawCardPayload Invalid Tests ==========

TEST(MessageSerializerTest, DeserializeDrawCardWithNullPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithArrayPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":[]})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithMissingDrawCount)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"cards":[]}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithMissingCards)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":5}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithStringDrawCount)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":"5"}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithNullDrawCount)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":null}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithNegativeDrawCount)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":-5}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithFloatDrawCount)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":5.5,"cards":[]}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithNullCards)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":5,"cards":null}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithStringCards)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":5,"cards":"invalid"}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithInvalidCardInArray)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":2,"cards":[{"card_color":"Red","card_type":"5"},"invalid"]}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithInvalidCardColor)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":1,"cards":[{"card_color":"Purple","card_type":"5"}]}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeDrawCardWithInvalidCardType)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":1,"cards":[{"card_color":"Red","card_type":"Invalid"}]}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

// ========== PlayCardPayload Invalid Tests ==========

TEST(MessageSerializerTest, DeserializePlayCardWithNullPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithArrayPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":[]})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithMissingCard)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithNullCard)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":null}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithArrayCard)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":[]}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithStringCard)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":"Red 5"}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithMissingCardColor)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_type":"5"}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithMissingCardType)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":"Red"}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithInvalidCardColor)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":"Purple","card_type":"5"}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithInvalidCardType)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":"Red","card_type":"Invalid"}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithNumberCardColor)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":123,"card_type":"5"}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithNumberCardType)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":"Red","card_type":5}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithNullCardColor)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":null,"card_type":"5"}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithNullCardType)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":"Red","card_type":null}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithEmptyCardColor)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":"","card_type":"5"}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithEmptyCardType)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":"Red","card_type":""}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithCaseSensitiveColor)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":"red","card_type":"5"}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializePlayCardWithCaseSensitiveType)
{
    std::string json = R"({"status_code":"OK","payload_type":"PLAY_CARD","payload":{"card":{"card_color":"Red","card_type":"skip"}}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

// ========== InitGamePayload Invalid Tests ==========

TEST(MessageSerializerTest, DeserializeInitGameWithNullPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"INIT_GAME","payload":null})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithArrayPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"INIT_GAME","payload":[]})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithMissingDiscardPile)
{
    std::string json = R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"hand_card":[],"current_player":0}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithMissingHandCard)
{
    std::string json = R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[],"current_player":0}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithMissingCurrentPlayer)
{
    std::string json = R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[],"hand_card":[]}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithNullDiscardPile)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":null,"hand_card":[],"current_player":0}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithNullHandCard)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[],"hand_card":null,"current_player":0}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithStringDiscardPile)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":"test","hand_card":[],"current_player":0}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithStringHandCard)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[],"hand_card":"test","current_player":0}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithStringCurrentPlayer)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[],"hand_card":[],"current_player":"0"}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithNullCurrentPlayer)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[],"hand_card":[],"current_player":null}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithNegativeCurrentPlayer)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[],"hand_card":[],"current_player":-1}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithFloatCurrentPlayer)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[],"hand_card":[],"current_player":1.5}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithInvalidCardInDiscardPile)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[{"card_color":"Invalid","card_type":"5"}],"hand_card":[],"current_player":0}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithInvalidCardInHandCard)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[],"hand_card":[{"card_color":"Red","card_type":"Invalid"}],"current_player":0}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithNonObjectCardInDiscardPile)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":["string"],"hand_card":[],"current_player":0}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeInitGameWithNonObjectCardInHandCard)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[],"hand_card":[123],"current_player":0}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

// ========== EndGamePayload Invalid Tests ==========

TEST(MessageSerializerTest, DeserializeEndGameWithNonNullPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"END_GAME","payload":{}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeEndGameWithArrayPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"END_GAME","payload":[]})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeEndGameWithStringPayload)
{
    std::string json = R"({"status_code":"OK","payload_type":"END_GAME","payload":"test"})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

// ========== Edge Cases ==========

TEST(MessageSerializerTest, DeserializeWithExtraFields)
{
    std::string json = R"({"status_code":"OK","payload_type":"START_GAME","payload":null,"extra_field":"ignored"})";
    Message message  = MessageSerializer::deserialize(json);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::START_GAME);
}

TEST(MessageSerializerTest, DeserializeJoinGameWithExtraPayloadFields)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":"Player1","extra":"ignored"}})";
    Message message  = MessageSerializer::deserialize(json);
    auto payload     = std::get<JoinGamePayload>(message.getMessagePayload());
    EXPECT_EQ(payload.playerName, "Player1");
}

TEST(MessageSerializerTest, DeserializeVeryLongPlayerName)
{
    std::string longName(10000, 'a');
    nlohmann::json json = {{"status_code", "OK"}, {"payload_type", "JOIN_GAME"}, {"payload", {{"name", longName}}}};
    Message message     = MessageSerializer::deserialize(json.dump());
    auto payload        = std::get<JoinGamePayload>(message.getMessagePayload());
    EXPECT_EQ(payload.playerName, longName);
}

TEST(MessageSerializerTest, DeserializeLargeDrawCount)
{
    std::string json = R"({"status_code":"OK","payload_type":"DRAW_CARD","payload":{"draw_count":2147483647,"cards":[]}})";
    Message message  = MessageSerializer::deserialize(json);
    auto payload     = std::get<DrawCardPayload>(message.getMessagePayload());
    EXPECT_EQ(payload.drawCount, 2147483647);
}

TEST(MessageSerializerTest, DeserializeLargeCurrentPlayer)
{
    std::string json =
        R"({"status_code":"OK","payload_type":"INIT_GAME","payload":{"discard_pile":[],"hand_card":[],"current_player":4294967295}})";
    Message message = MessageSerializer::deserialize(json);
    auto payload    = std::get<InitGamePayload>(message.getMessagePayload());
    EXPECT_EQ(payload.currentPlayerIndex, 4294967295);
}

TEST(MessageSerializerTest, DeserializeUnicodePlayerName)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":"玩家123🎮"}})";
    Message message  = MessageSerializer::deserialize(json);
    auto payload     = std::get<JoinGamePayload>(message.getMessagePayload());
    EXPECT_EQ(payload.playerName, "玩家123🎮");
}

TEST(MessageSerializerTest, DeserializeWithWhitespace)
{
    std::string json = R"(
        {
            "status_code": "OK",
            "payload_type" : "START_GAME" ,
            "payload" : null
        }
    )";
    Message message  = MessageSerializer::deserialize(json);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::START_GAME);
}

TEST(MessageSerializerTest, DeserializeCompactJSON)
{
    std::string json = R"({"status_code":"OK","payload_type":"START_GAME","payload":null})";
    Message message  = MessageSerializer::deserialize(json);
    EXPECT_EQ(message.getMessagePayloadType(), MessagePayloadType::START_GAME);
}

TEST(MessageSerializerTest, DeserializeAllCardColors)
{
    std::vector<std::pair<std::string, CardColor>> colors = {
        {"Red", CardColor::RED}, {"Blue", CardColor::BLUE}, {"Green", CardColor::GREEN}, {"Yellow", CardColor::YELLOW}};

    for (const auto &[colorStr, colorEnum] : colors) {
        nlohmann::json json = {
            {"status_code", "OK"}, {"payload_type", "PLAY_CARD"}, {"payload", {{"card", {{"card_color", colorStr}, {"card_type", "5"}}}}}};
        Message message = MessageSerializer::deserialize(json.dump());
        auto payload    = std::get<PlayCardPayload>(message.getMessagePayload());
        EXPECT_EQ(payload.card.getColor(), colorEnum);
    }
}

TEST(MessageSerializerTest, DeserializeAllCardTypes)
{
    std::vector<std::pair<std::string, CardType>> types = {{"0", CardType::NUM0},
                                                           {"1", CardType::NUM1},
                                                           {"2", CardType::NUM2},
                                                           {"3", CardType::NUM3},
                                                           {"4", CardType::NUM4},
                                                           {"5", CardType::NUM5},
                                                           {"6", CardType::NUM6},
                                                           {"7", CardType::NUM7},
                                                           {"8", CardType::NUM8},
                                                           {"9", CardType::NUM9},
                                                           {"Skip", CardType::SKIP},
                                                           {"Reverse", CardType::REVERSE},
                                                           {"Draw 2", CardType::DRAW2},
                                                           {"Wild", CardType::WILD},
                                                           {"Wild Draw 4", CardType::WILDDRAWFOUR}};

    for (const auto &[typeStr, typeEnum] : types) {
        nlohmann::json json = {
            {"status_code", "OK"}, {"payload_type", "PLAY_CARD"}, {"payload", {{"card", {{"card_color", "Red"}, {"card_type", typeStr}}}}}};
        Message message = MessageSerializer::deserialize(json.dump());
        auto payload    = std::get<PlayCardPayload>(message.getMessagePayload());
        EXPECT_EQ(payload.card.getType(), typeEnum);
    }
}

// ========== Security Tests ==========

TEST(MessageSerializerTest, DeserializeJSONBomb)
{
    // Test deeply nested JSON (potential DoS)
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":"player"}})";
    for (int i = 0; i < 100; ++i) {
        json = std::string(R"({"nested":)").append(json).append("}");
    }
    EXPECT_THROW(MessageSerializer::deserialize(json), std::exception);
}

TEST(MessageSerializerTest, DeserializeHugeArray)
{
    // Test with a very large array
    nlohmann::json json;
    json["status_code"]             = "OK";
    json["payload_type"]            = "INIT_GAME";
    json["payload"]["discard_pile"] = nlohmann::json::array();
    for (int i = 0; i < 10000; ++i) {
        json["payload"]["discard_pile"].push_back({{"card_color", "Red"}, {"card_type", "5"}});
    }
    json["payload"]["hand_card"]      = nlohmann::json::array();
    json["payload"]["current_player"] = 0;

    EXPECT_NO_THROW(MessageSerializer::deserialize(json.dump()));
}

TEST(MessageSerializerTest, DeserializeNullCharacterInString)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":"Player)" + std::string(1, '\0') + R"(1"}})";
    EXPECT_THROW(MessageSerializer::deserialize(json), std::invalid_argument);
}

TEST(MessageSerializerTest, DeserializeControlCharactersInString)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":"Player\u0001\u001f"}})";
    Message message  = MessageSerializer::deserialize(json);
    auto payload     = std::get<JoinGamePayload>(message.getMessagePayload());
    EXPECT_EQ(payload.playerName.length(), 8);
}

TEST(MessageSerializerTest, DeserializeSQLInjectionAttempt)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":"'; DROP TABLE users; --"}})";
    Message message  = MessageSerializer::deserialize(json);
    auto payload     = std::get<JoinGamePayload>(message.getMessagePayload());
    EXPECT_EQ(payload.playerName, "'; DROP TABLE users; --");
}

TEST(MessageSerializerTest, DeserializeXSSAttempt)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":"<script>alert('XSS')</script>"}})";
    Message message  = MessageSerializer::deserialize(json);
    auto payload     = std::get<JoinGamePayload>(message.getMessagePayload());
    EXPECT_EQ(payload.playerName, "<script>alert('XSS')</script>");
}

TEST(MessageSerializerTest, DeserializePathTraversalAttempt)
{
    std::string json = R"({"status_code":"OK","payload_type":"JOIN_GAME","payload":{"name":"../../etc/passwd"}})";
    Message message  = MessageSerializer::deserialize(json);
    auto payload     = std::get<JoinGamePayload>(message.getMessagePayload());
    EXPECT_EQ(payload.playerName, "../../etc/passwd");
}

