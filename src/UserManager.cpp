#include "UserManager.hpp"
#include <iostream>
#include <unistd.h>
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

void UserManager::broadcastMessage(int senderSocket, const std::string &message)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto &[client, clientUsernames] : clientUsernames)
    {
        send(client, message.c_str(), message.size(), 0);
    }
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
