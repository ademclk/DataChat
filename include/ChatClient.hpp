#ifndef CHATCLIENT_HPP
#define CHATCLIENT_HPP

#include <string>

class ChatClient
{
public:
    ChatClient();
    ~ChatClient();

    void startChat();

private:
    int clientSocket;
    std::string username;

    void handleUserInput();
    void handleClientCommand(const std::string &command);
    void updateUsername(const std::string &newUsername);
    void sendMessage(const std::string &message);
    void receiveMessages();
    void clearLine();
    void closeConnection();
};

#endif
