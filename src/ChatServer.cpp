#include "ChatServer.hpp"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

ChatServer::ChatServer() : isServerRunning(true)
{
}

ChatServer::~ChatServer()
{
    stopServer();
}

void ChatServer::startListening()
{
    while (isServerRunning)
    {
        try
        {
            int clientSocket = serverSocket.acceptConnection();
            sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);

            getpeername(clientSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);

            std::cout << "Received connection request from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

            auto handler = std::make_shared<ClientHandler>(clientSocket, std::ref(userManager));
            clientThreads.emplace_back(&ClientHandler::handle, handler); // Add the thread to the vector
        }
        catch (const std::runtime_error &e)
        {
            std::cerr << "Failed to accept connection: " << e.what() << std::endl;
        }
    }
}

void ChatServer::stopServer() {  
    isServerRunning = false;
  
    for (auto& thread : clientThreads) {  
        if (thread.joinable()) {  
            thread.join();  
        }  
    }  
  
    clientThreads.clear();
}  

void ChatServer::handleClient(int clientSocket)
{
    // Implement the existing handleClient function logic here
}
