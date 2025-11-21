/**
 * @file
 *
 * @author Yuzhe Guo
 * @date 2025.11.18
 */
#ifndef UNO_GAME_MESSAGE_H
#define UNO_GAME_MESSAGE_H
#include "../game/Card.h"
#include "../game/CardTile.h"
#include "../game/Player.h"


#include <string>
#include <variant>


namespace UNO::NETWORK {
    enum class MessagePayloadType { JOIN_GAME, START_GAME, DRAW_CARD, PLAY_CARD, INIT_GAME, END_GAME };

    struct JoinGamePayload {
        std::string playerName;
    };

    struct StartGamePayload {};

    struct DrawCardPayload {
        int drawCount;
    };

    struct PlayCardPayload {
        GAME::Card card;
    };

    struct InitGamePayload {
        GAME::DiscardPile discardPile;
        GAME::HandCard handCard;
        size_t currentPlayerIndex;
    };

    struct EndGamePayload {};

    using MessagePayload =
        std::variant<std::monostate, JoinGamePayload, StartGamePayload, DrawCardPayload, PlayCardPayload, InitGamePayload, EndGamePayload>;

    class Message {
    private:
        MessagePayloadType messagePayloadType_;
        MessagePayload messagePayload_;

    public:
        Message(MessagePayloadType messagePayloadType, MessagePayload messagePayload);

        [[nodiscard]] MessagePayloadType getMessagePayloadType() const;
        [[nodiscard]] MessagePayload getMessagePayload() const;
    };

}   // namespace UNO::NETWORK

#endif   // UNO_GAME_MESSAGE_H
