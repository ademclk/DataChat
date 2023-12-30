#include "Message.hpp"
#include <stdexcept>
#include <iostream>
#include <bitset>

/**
 * @brief Construct a new Message:: Message object
 *
 * @param content The content of the message.
 * @param sender The sender of the message.
 */
Message::Message(const std::string &content, const std::string &sender, CommandType commandType)
    : content(content), sender(sender), commandType(commandType)
{
    calculateParityBit();
    calculateCRC();
}

/**
 * @brief Get the content of the message.
 *
 * @return string | The content of the message.
 */
std::string Message::getContent() const
{
    return content;
}

std::string Message::getSenderUsername() const
{
    return sender;
}

/**
 * @brief Get the formatted message.
 *
 * @return std::string The formatted message.
 */
std::string Message::getFormattedMessage() const
{
    std::string commandString;
    switch (commandType)
    {
    case CommandType::MESG:
        commandString = "MESG";
        break;
    case CommandType::CONN:
        commandString = "CONN";
        break;
    case CommandType::MERR:
        commandString = "MERR";
        break;
    case CommandType::GONE:
        commandString = "GONE";
        break;
    case CommandType::PRIV:
        commandString = "PRIV";
        break;
    default:
        commandString = "NONE";
    }

    return sender + " | " + commandString + " | " + content + " | [" + parityBits + ":" + crcBits + "]";
}

CommandType Message::getCommandType() const
{
    return commandType;
}

int Message::countOnes(const std::string &str)
{
    int numberOfOnes = 0;

    for (char c : str)
    {
        numberOfOnes += std::bitset<8>(c).count();
    }

    return numberOfOnes;
}

void Message::corruptMessage()
{
    if (commandType == CommandType::MESG && content.size() > 0)
    {
        int index = rand() % content.size(); // Random index
        char bitToFlip = 1 << (rand() % 8);  // Random bit
        content[index] ^= bitToFlip;         // Flip bit
    }
}

void Message::calculateParityBit()
{
    int onesCount = countOnes(content);
    parityBits = (onesCount % 2 == 1) ? "1" : "0";
}

bool Message::verifyParityBit() const
{
    int onesCount = countOnes(content);
    return (onesCount + std::stoi(parityBits)) % 2 == 0;
}

// void Message::calculateParity2D()
// {
//     int rows = content.length();
//     int columns = 8; // Assume each character is 8 bits
//     std::vector<std::bitset<8>> bits(rows);

//     for (int i = 0; i < rows; ++i)
//     {
//         bits[i] = std::bitset<8>(content[i]);
//     }

//     std::string rowParityBits, columnParityBits(columns, '0');
//     for (const auto &row : bits)
//     {
//         rowParityBits.push_back((row.count() % 2 == 1) ? '1' : '0');
//         for (int i = 0; i < columns; ++i)
//         {
//             if (row[i] == 1)
//             {
//                 columnParityBits[i] = (columnParityBits[i] == '0') ? '1' : '0';
//             }
//         }
//     }

//     errorCheckingBits = rowParityBits + columnParityBits;
// }

// bool Message::verifyParityBit2D() const
// {
//     int rows = content.length();
//     int columns = 8;
//     std::vector<std::bitset<8>> bits(rows);

//     for (int i = 0; i < rows; ++i)
//     {
//         bits[i] = std::bitset<8>(content[i]);
//     }

//     std::string rowParityBits, columnParityBits(columns, '0');
//     for (const auto &row : bits)
//     {
//         rowParityBits.push_back((row.count() % 2 == 1) ? '1' : '0');
//         for (int i = 0; i < columns; ++i)
//         {
//             if (row[i] == 1)
//             {
//                 columnParityBits[i] = (columnParityBits[i] == '0') ? '1' : '0';
//             }
//         }
//     }

//     return errorCheckingBits == rowParityBits + columnParityBits;
// }

// void Message::calculateChecksum()
// {
//     uint16_t sum = 0;
//     for (char c : content)
//     {
//         sum += static_cast<uint16_t>(c);
//     }
//     sum = ~sum; // one's complement of sum
//     std::bitset<16> bits(sum);
//     errorCheckingBits = bits.to_string();
// }

// bool Message::verifyChecksum() const
// {
//     uint16_t sum = 0;
//     for (char c : content)
//     {
//         sum += static_cast<uint16_t>(c);
//     }
//     sum = ~sum;
//     std::bitset<16> bits(sum);
//     return errorCheckingBits == bits.to_string();
// }

void Message::calculateCRC()
{
    // Generator used here is x^3 + x + 1
    const std::string generator = "1011";
    std::string data = std::bitset<8>(content[0]).to_string();
    for (int i = 1; i < content.size(); ++i)
    {
        data += std::bitset<8>(content[i]).to_string();
    }

    // Add 0s one less degree of the generator polynomial
    data = data + std::string(generator.size() - 1, '0');

    // Using XOR (to substract generator from to polynomial) 
    // for the division part of the CRC algorithm
    for (int i = 0; i <= data.size() - generator.size();)
    {
        for (int j = 0; j < generator.size(); ++j)
        {
            data[i + j] = data[i + j] == generator[j] ? '0' : '1';
        }
        for (; i < data.size() && data[i] != '1'; ++i)
        {
        }
    }

    crcBits = data.substr(data.size() - generator.size() + 1);
}

bool Message::verifyCRC() const
{
    const std::string generator = "1011";
    std::string data = std::bitset<8>(content[0]).to_string();
    for (int i = 1; i < content.size(); ++i)
    {
        data += std::bitset<8>(content[i]).to_string();
    }

    data = data + crcBits;

    for (int i = 0; i <= data.size() - generator.size();)
    {
        for (int j = 0; j < generator.size(); ++j)
        {
            data[i + j] = data[i + j] == generator[j] ? '0' : '1';
        }
        for (; i < data.size() && data[i] != '1'; ++i)
        {
        }
    }

    return data.substr(data.size() - generator.size() + 1) == std::string(generator.size() - 1, '0');
}

Message Message::parseFromString(const std::string &rawMessage)
{
    size_t senderEndPos = rawMessage.find('|');
    if (senderEndPos == std::string::npos)
    {
        throw std::runtime_error("Invalid message format");
    }
    std::string sender = rawMessage.substr(0, senderEndPos - 1); // -1 to skip " "

    size_t commandEndPos = rawMessage.find('|', senderEndPos + 1);
    if (commandEndPos == std::string::npos)
    {
        throw std::runtime_error("Invalid message format");
    }
    std::string commandString = rawMessage.substr(senderEndPos + 2, commandEndPos - senderEndPos - 3); // +2 to skip " |", -3 to skip " |"

    CommandType commandType;
    if (commandString == "MESG")
    {
        commandType = CommandType::MESG;
    }
    else if (commandString == "CONN")
    {
        commandType = CommandType::CONN;
    }
    else if (commandString == "MERR")
    {
        commandType = CommandType::MERR;
    }
    else if (commandString == "GONE")
    {
        commandType = CommandType::GONE;
    }
    else if (commandString == "PRIV")
    {
        commandType = CommandType::PRIV;
    }
    else
    {
        commandType = CommandType::NONE;
    }

    size_t separatorPos = rawMessage.find_last_of('[');
    size_t endPos = rawMessage.find_last_of(']');
    if (separatorPos == std::string::npos || endPos == std::string::npos || separatorPos > endPos)
    {
        throw std::runtime_error("Invalid message format");
    }

    std::string content = rawMessage.substr(commandEndPos + 2, separatorPos - commandEndPos - 5); // +2 to skip " |", -4 to skip " | ["
    std::string parityBit = rawMessage.substr(separatorPos + 1, endPos - separatorPos - 1);

    // Construct a message object with the parsed content
    Message message(content, sender, commandType);

    std::string errorBits = rawMessage.substr(separatorPos + 1, endPos - separatorPos - 1);
    size_t separatorPos2 = errorBits.find(':');
    if (separatorPos2 == std::string::npos || separatorPos2 >= errorBits.size() - 1)
    {
        throw std::runtime_error("Invalid message format");
    }
    message.parityBits = errorBits.substr(0, separatorPos2);
    message.crcBits = errorBits.substr(separatorPos2 + 1);

    // Verify the parity bit
    if (!message.verifyParityBit())
    {
        throw std::runtime_error("Message verification failed: Parity bit does not match content");
    }

    // Verify CRC
    if (!message.verifyCRC())
    {
        throw std::runtime_error("Message verification failed: CRC does not match content");
    }

    return message;
}
