/**
 * @file
 *
 * @author Yuzhe Guo
 * @date 2025.11.18
 */
#include "Message.h"

#include <stdexcept>
#include <utility>

namespace UNO::NETWORK {
    Message::Message(MessageStatus messageStatus, MessagePayloadType messagePayloadType, MessagePayload messagePayload) :
        status_(messageStatus), messagePayloadType_(messagePayloadType), messagePayload_(std::move(messagePayload))
    {
        if (this->getMessageStatus() == MessageStatus::INVALID) {
            if (this->getMessagePayloadType() != MessagePayloadType::EMPTY) {
                throw std::invalid_argument("Invalid message: expected 'empty' payload type");
            }
            if (std::holds_alternative<std::monostate>(this->getMessagePayload()) == false) {
                throw std::invalid_argument("Invalid message: expected std::monostate payload");
            }
        }
        else {
            if ((this->getMessagePayloadType() == MessagePayloadType::JOIN_GAME
                 && std::holds_alternative<JoinGamePayload>(this->getMessagePayload()) == false)
                || (this->getMessagePayloadType() == MessagePayloadType::START_GAME
                    && std::holds_alternative<StartGamePayload>(this->getMessagePayload()) == false)
                || (this->getMessagePayloadType() == MessagePayloadType::DRAW_CARD
                    && std::holds_alternative<DrawCardPayload>(this->getMessagePayload()) == false)
                || (this->getMessagePayloadType() == MessagePayloadType::PLAY_CARD
                    && std::holds_alternative<PlayCardPayload>(this->getMessagePayload()) == false)
                || (this->getMessagePayloadType() == MessagePayloadType::INIT_GAME
                    && std::holds_alternative<InitGamePayload>(this->getMessagePayload()) == false)
                || (this->getMessagePayloadType() == MessagePayloadType::END_GAME
                    && std::holds_alternative<EndGamePayload>(this->getMessagePayload()) == false)) {
                throw std::invalid_argument("Invalid message: MessagePayloadType and MessagePayload do not match");
            }
        }
    }

    MessageStatus Message::getMessageStatus() const
    {
        return this->status_;
    }

    MessagePayloadType Message::getMessagePayloadType() const
    {
        return this->messagePayloadType_;
    }

    MessagePayload Message::getMessagePayload() const
    {
        return this->messagePayload_;
    }


}   // namespace UNO::NETWORK