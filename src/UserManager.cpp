#include "UserManager.hpp"
#include "../include/SocketUtils.hpp"
#include <iostream>
#include <unistd.h>
#include <../common/Message.hpp>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

UserManager::UserManager()
{
}

UserManager::~UserManager()
{
}

void UserManager::updateUsernames(int clientSocket, const std::string &newUsername)
{
    std::lock_guard<std::mutex> lock(mutex);
    clientUsernames[clientSocket] = newUsername;
}

std::string UserManager::getUsername(int clientSocket) const
{
    std::lock_guard<std::mutex> lock(mutex);
    auto usernameIterator = clientUsernames.find(clientSocket);
    return (usernameIterator != clientUsernames.end()) ? usernameIterator->second : "Guest";
}

int UserManager::getClientSocket(const std::string &username) const
{
    std::lock_guard<std::mutex> lock(mutex); // Lock the mutex as we're accessing shared data.
    std::cout << "Getting client socket for username: " << username << std::endl;
    for (const auto &clientUsername : clientUsernames)
    {
        if (clientUsername.second == username)
        {
            return clientUsername.first;
        }
    }
    return -1; // Return -1 or another invalid socket number if the username was not found.
}

void UserManager::broadcastMessage(int senderSocket, const std::string &message)
{
    std::string senderUsername;
    if (senderSocket == -2)
    {
        senderUsername = "SYSTEM";
    }
    else
    {
        senderUsername = getUsername(senderSocket);
    }
    std::cout << "Broadcasting message for all clients..." << std::endl;
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto &[client, clientUsername] : clientUsernames)
    {
        Message chatMessage(message, senderUsername, CommandType::MESG);
        std::string chatMessageStr = chatMessage.getFormattedMessage();
        std::cout << "Message successfully sent to " << client << "::" << clientUsername << std::endl;

        sendDelimitedMessage(client, chatMessageStr);
    }
}

void UserManager::sendPrivateMessage(int senderSocket, int recipientSocket, const std::string &message)
{
    std::string senderUsername;
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto senderIter = clientUsernames.find(senderSocket);
        if (senderIter != clientUsernames.end())
        {
            senderUsername = senderIter->second;
        }
        else
        {
            return;
        }
    }
    std::cout << "Sending private message from " << senderUsername << " to " << recipientSocket << ": " << message << std::endl;
    std::string formattedMessage = senderUsername + " (private): " + message;
    sendDelimitedMessage(recipientSocket, formattedMessage);
}

void UserManager::removeClient(int clientSocket)
{
    std::lock_guard<std::mutex> lock(mutex);
    clientUsernames.erase(clientSocket);
}

std::vector<std::string> UserManager::getOnlineUsernames() const
{
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> onlineUsernames;

    for (const auto &[client, username] : clientUsernames)
    {
        onlineUsernames.push_back(username);
    }

    return onlineUsernames;
}
