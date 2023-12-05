#include "Message.hpp"

/**
 * @brief Construct a new Message:: Message object
 *
 * @param content The content of the message.
 * @param sender The sender of the message.
 */
Message::Message(const std::string &content, const std::string &sender)
    : content(content), sender(sender) {}

/**
 * @brief Get the content of the message.
 *
 * @return string | The content of the message.
 */
std::string Message::getContent() const
{
    return content;
}

/**
 * @brief Get the formatted message.
 *
 * @return std::string The formatted message.
 */
std::string Message::getFormattedMessage() const
{
    return sender + ": " + content;
}
