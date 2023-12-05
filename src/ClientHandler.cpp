#include "ClientHandler.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

ClientHandler::ClientHandler(int clientSocket, UserManager &userManager)
    : clientSocket(clientSocket), userManager(userManager)
{
}

void ClientHandler::handle()
{
    // Send a connection message to the client
    const char *connectionMessage = "SYSTEM | 200 | Welcome to the chat! Please set your username using !username <your_username>.";
    send(clientSocket, connectionMessage, strlen(connectionMessage), 0);

    // Buffer to store the received messages
    char buffer[4096];

    // Default username for the client
    std::string username = "Guest";

    // Keep handling the client connection until the client disconnects or quits
    while (!hasSetUsername)
    {
        // Receive message from the client
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            if (bytesReceived == 0)
            {
                // Client closed the connection
                std::cout << "Client " << username << " disconnected." << std::endl;
            }
            else
            {
                perror("recv: ");
            }

            break;
        }
        // Null-terminate the received message
        buffer[bytesReceived] = '\0';

        // Convert the received message to a string
        std::string clientMessage = buffer;

        if (clientMessage.substr(0, 9) == "!username")
        {
            std::string newUsername = clientMessage.substr(9);
            username = newUsername;
            userManager.updateUsernames(clientSocket, username);
            hasSetUsername = true;

            std::string successMessage = "SYSTEM | 200 | Username set to " + username + ". You can now start chatting!";
            std::cout << successMessage << std::endl;

            // Broadcast to all clients that the user has set a username
            userManager.broadcastMessage(clientSocket, successMessage);

            // Send help message
            sendHelpMessage();
        }
        else
        {
            // Inform the client to set a username
            send(clientSocket, "SYSTEM | 200 | Please set your username using !username <your_username>.", 72, 0);
        }
    }

    while (true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            if (bytesReceived == 0)
            {
                // Client closed the connection
                std::cout << "Client " << username << " disconnected." << std::endl;
            }
            else
            {
                perror("recv: ");
            }

            break;
        }

        buffer[bytesReceived] = '\0';
        std::string clientMessage = buffer;

        if (clientMessage.substr(0, 1) == "!")
        {
            handleCommand(clientMessage, username);
        }
        else
        {
            handleRegularMessage(clientMessage, username);
        }
    }

    // Close the client socket
    close(clientSocket);
}

void ClientHandler::handleCommand(const std::string &command, std::string &username)
{
    // If the client sent a quit command, print a quit message, broadcast it to all other clients, and break the loop
    if (command == "!quit")
    {
        std::string quitMessage = "SYSTEM | 200 | " + username + " has quit.";
        std::cout << quitMessage << std::endl;

        userManager.broadcastMessage(clientSocket, quitMessage);
        userManager.removeClient(clientSocket);

        // Break the loop
        return;
    }
    // If the client sent a username command, update the username, print a username update message,
    // and broadcast it to all other clients
    else if (command.substr(0, 9) == "!username")
    {
        std::string oldUsername = username;
        username = command.substr(9);

        userManager.updateUsernames(clientSocket, username);

        std::string usernameMessage = "SYSTEM | 200 | " + oldUsername + " updated username to " + username + ".";
        std::cout << usernameMessage << std::endl;

        userManager.broadcastMessage(clientSocket, usernameMessage);
    }
    else if (command.substr(0, 9) == "!help")
    {
        std::cout << "send help message.";
        sendHelpMessage();
    }
    // Add other command handlers as needed
}

void ClientHandler::handleRegularMessage(const std::string &message, const std::string &username)
{
    // If the client sent a regular message, print it and broadcast it to all clients
    std::cout << "Received message from " << username << ": " << message << std::endl;

    // Check if the message already contains the sender's username
    std::string clientMessage = message;
    if (clientMessage.find(username) == std::string::npos)
    {
        // If the sender's username is not present, prepend it
        clientMessage = username + ": " + message;
    }

    // Broadcast the message to all clients, including the sender
    userManager.broadcastMessage(clientSocket, clientMessage);
}

void ClientHandler::sendHelpMessage()
{
    const char *helpMessage = "\nSYSTEM | 200 | Available Commands:\n"
                              "!help                - Display this help message\n"
                              "!quit                - Quit the chat\n"
                              "!username <new_name> - Change your username\n";

    send(clientSocket, helpMessage, strlen(helpMessage), 0);
}