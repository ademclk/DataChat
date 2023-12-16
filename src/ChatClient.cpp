#include "ChatClient.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <mutex>
#include <thread>

ChatClient::ChatClient() : clientSocket(-1), username("Guest")
{
    // Implement any necessary initialization
}

ChatClient::~ChatClient()
{
    // Implement any necessary cleanup
}

void ChatClient::clearLine()
{
    std::cout << "\x1B[K" << std::flush;
}

void ChatClient::receiveMessages()
{
    char buffer[4096];
    while (true)
    {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            std::cout << "Disconnected from server." << std::endl;
            break;
        }
        buffer[bytesRead] = '\0';

        std::cout << "Data received: " << buffer << std::endl; // Add this line

        clearLine();
        std::cout << "Received message: " << buffer << std::endl;
        std::cout << buffer << std::endl;
    }
}

void ChatClient::handleUserInput()
{
    while (true)
    {
        std::string message;
        getline(std::cin, message);

        std::cout << "User input: " << message << std::endl;

        if (message.substr(0, 1) == "!")
        {
            handleClientCommand(message);
        }
        else
        {
            sendMessage(username + ": " + message);
        }
    }
}

void ChatClient::handleClientCommand(const std::string &command)
{
    std::cout << "Handling command: " << command << std::endl;
    if (command == "!quit")
    {
        std::cout << "Quitting..." << std::endl;
        sendMessage(command);
        closeConnection();
        return;
    }

    if (command.substr(0, 9) == "!username")
    {
        updateUsername(command.substr(10));
    }

    if (command == "!help")
    {
        sendMessage(command);
    }

    if (command == "!list")
    {
        sendMessage(command);
    }

    if (command.substr(0, 8) == "!private")
    {
        sendMessage(command);
    }
}

void ChatClient::updateUsername(const std::string &newUsername)
{
    std::string oldUsername = username;
    username = newUsername;
    std::cout << "Updated username to " << username << "." << std::endl;
    sendMessage("!username" + newUsername);
}

void ChatClient::sendMessage(const std::string &message)
{
    clearLine();
    std::cout << "\x1B[A"; // Move up one line
    std::cout << "\x1B[K"; // Clear the line
    send(clientSocket, message.c_str(), message.size(), 0);
}

void ChatClient::startChat()
{
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Socket " << clientSocket << " created." << std::endl;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Connection failed");
        close(clientSocket);
        return;
    }

    char buffer[4096];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    std::cout << "Connecting to server..." << std::endl;
    std::cout << buffer << std::endl;

    std::thread receiveThread(&ChatClient::receiveMessages, this);

    handleUserInput();

    receiveThread.join(); // Wait for the receive thread to finish

    closeConnection();
}

void ChatClient::closeConnection()
{
    close(clientSocket);
}
