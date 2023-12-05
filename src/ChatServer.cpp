#include "ChatServer.hpp"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

ChatServer::ChatServer()
{
}

ChatServer::~ChatServer()
{
}

void ChatServer::startListening()
{
    while (true)
    {
        // Accept a client connection
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket.getSocket(), (struct sockaddr *)&clientAddr, &clientAddrSize);

        // Create a new thread to handle the client
        std::cout << "Received connection request from " << inet_ntoa(clientAddr.sin_addr) << ":" << clientSocket << std::endl;

        std::thread clientThread(&ClientHandler::handle, ClientHandler(clientSocket, userManager));
        clientThread.detach(); // Detach the thread
    }
}

void ChatServer::handleClient(int clientSocket)
{
    // Implement the existing handleClient function logic here
}
