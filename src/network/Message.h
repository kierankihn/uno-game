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
#include <vector>


namespace UNO::NETWORK {
    enum class MessagePayloadType { EMPTY, JOIN_GAME, START_GAME, DRAW_CARD, PLAY_CARD, INIT_GAME, END_GAME };

    struct JoinGamePayload {
        std::string playerName;
    };

    struct StartGamePayload {};

    struct DrawCardPayload {
        size_t drawCount;
        std::vector<GAME::Card> cards;
    };

    struct PlayerPublicState {
        std::string name;
        size_t remainingCardCount;
        bool isUno;
    };

    struct PlayCardPayload {
        GAME::Card card;
    };

    struct InitGamePayload {
        size_t playerId;
        std::vector<PlayerPublicState> players;
        GAME::DiscardPile discardPile;
        std::multiset<GAME::Card> handCard;
        size_t currentPlayerIndex;
    };

    struct EndGamePayload {};

    using MessagePayload =
        std::variant<std::monostate, JoinGamePayload, StartGamePayload, DrawCardPayload, PlayCardPayload, InitGamePayload, EndGamePayload>;

    enum class MessageStatus { OK, INVALID };

    class Message {
    private:
        MessageStatus status_;

        MessagePayloadType messagePayloadType_;
        MessagePayload messagePayload_;

    public:
        Message(MessageStatus messageStatus, MessagePayloadType messagePayloadType, MessagePayload messagePayload);

        [[nodiscard]] MessageStatus getMessageStatus() const;
        [[nodiscard]] MessagePayloadType getMessagePayloadType() const;
        [[nodiscard]] MessagePayload getMessagePayload() const;
    };

}   // namespace UNO::NETWORK

#endif   // UNO_GAME_MESSAGE_H
