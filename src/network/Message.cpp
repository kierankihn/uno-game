/**
 * @file
 *
 * @author Yuzhe Guo
 * @date 2025.11.18
 */
#include "Message.h"

#include <utility>

namespace UNO::NETWORK {
    Message::Message(MessagePayloadType messagePayloadType, MessagePayload messagePayload) :
        messagePayloadType_(messagePayloadType), messagePayload_(std::move(messagePayload))
    {
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