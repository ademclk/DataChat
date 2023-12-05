#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>

/**
 * @class Message
 * @brief A class to encapsulate a message.
 *
 * This class encapsulates a message, which consists of content and a sender.
 */
class Message
{
public:
    /**
     * @brief Construct a new Message object.
     *
     * This constructor creates a new Message object with the given content and sender.
     *
     * @param content The content of the message.
     * @param sender The sender (identified by the username) of the message.
     */
    Message(const std::string &content, const std::string &sender);

    /**
     * @brief Get the content of the message.
     *
     * This function returns the content of the message.
     *
     * @return string | The content of the message.
     */
    std::string getContent() const;

    /**
     * @brief Get the formatted message.
     *
     * This function returns the formatted message, which includes the sender and the content.
     *
     * @return string | The formatted message.
     */
    std::string getFormattedMessage() const;

private:
    std::string content; // The content of the message.
    std::string sender;  // The sender (identified by the username) of the message.
};

#endif
