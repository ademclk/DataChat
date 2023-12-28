#ifndef CHATCLIENT_HPP
#define CHATCLIENT_HPP

#include <string>
#include <mutex>
#include <atomic>
#include <../common/Message.hpp>

class ChatClient
{
public:
    ChatClient();
    ~ChatClient();

    void startChat();

private:
    int clientSocket;
    std::string username;
    std::mutex consoleMutex;
    std::atomic<bool> shouldContinue;

    void handleUserInput();
    void handleClientCommand(const std::string &command);
    void updateUsername(const std::string &newUsername);
    void sendMessage(const Message &message);
    void receiveMessages();
    void clearLine();
    void closeConnection();
};

#endif
