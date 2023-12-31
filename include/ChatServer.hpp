#ifndef CHATSERVER_HPP
#define CHATSERVER_HPP

#include "ServerSocket.hpp"
#include "ClientHandler.hpp"
#include "UserManager.hpp"

class ChatServer
{
public:
    ChatServer();
    ~ChatServer();
    void startListening();
    void stopServer();

private:
    void handleClient(int clientSocket);

    ServerSocket serverSocket; // Use ServerSocket as a member variable
    UserManager userManager;   // The user manager to handle user-related actions
    std::vector<std::thread> clientThreads;
    bool isServerRunning;
};

#endif
