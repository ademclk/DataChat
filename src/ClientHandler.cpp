#include "ClientHandler.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <thread>

ClientHandler::ClientHandler(int clientSocket, UserManager &userManager)
    : clientSocket(clientSocket), userManager(userManager), hasSetUsername(false)
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
        std::cout << "Setting username" << std::endl;
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
        std::cout << "First clientMessage" << std::endl;
        std::cout << clientMessage << std::endl;
        Message receivedMessage = Message::parseFromString(clientMessage);
        std::cout << "Parsed client message" << std::endl;
        std::cout << receivedMessage.getFormattedMessage() << std::endl;

        if (clientMessage.substr(0, 9) == "!username")
        {
            std::string newUsername = clientMessage.substr(9);
            username = newUsername;
            userManager.updateUsernames(clientSocket, username);
            hasSetUsername = true;

            // Notify the original sender client about the username change
            std::string successMessage = "SYSTEM | 200 | Username set to " + username + ". You can now start chatting!\n";
            std::cout << successMessage << std::endl;
            send(clientSocket, successMessage.c_str(), successMessage.size(), 0);

            // Broadcast to all clients (excluding the original sender) that the user has set a username
            std::string broadcastMessage = "SYSTEM | 200 | User " + username + " has joined the chat!";
            userManager.broadcastMessage(clientSocket, broadcastMessage);

            // Send help message
            sendHelpMessage();
        }
        else
        {
            std::cout << "clientMessage" << std::endl;

            std::cout << clientMessage << std::endl;
            // Inform the client to set a username
            Message setUsernameMessage("Set your username using !username <username>", "SYSTEM", CommandType::MESG);
            std::string messageStr = setUsernameMessage.getFormattedMessage();
            send(clientSocket, messageStr.c_str(), messageStr.size(), 0);
        }
    }

    while (true)
    {
        std::cout << "Username not set" << std::endl;
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
    userManager.removeClient(clientSocket);
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
    else if (command.rfind("!username ", 0) == 0)
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
        sendHelpMessage();
    }
    else if (command.substr(0, 9) == "!list")
    {
        listUsers();
    }
    else if (command.rfind("!private ", 0) == 0)
    {
        std::string rest = command.substr(9);
        std::size_t pos = rest.find(" ");
        std::string targetUsername = rest.substr(0, pos);
        std::string message = rest.substr(pos + 1);

        // Get the target client's socket
        int targetSocket = userManager.getClientSocket(targetUsername);

        if (targetSocket != -1) // If the target client's socket is valid
        {
            // This line blocks all clients and server
            // userManager.sendPrivateMessage(clientSocket, targetSocket, message);

            // Ensure sendPrivateMessage does not block
            std::thread([this, targetSocket, message]()
                        { userManager.sendPrivateMessage(clientSocket, targetSocket, message); })
                .detach();
        }
        else
        {
            std::string errorMessage = "User " + targetUsername + " does not exist or is not connected.";
            send(clientSocket, errorMessage.c_str(), errorMessage.size(), 0);
        }
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
                              "!username <new_name> - Change your username\n"
                              "!list                - List of online users\n"
                              "!quit                - Quit the chat\n";

    send(clientSocket, helpMessage, strlen(helpMessage), 0);
}

void ClientHandler::listUsers()
{
    std::string userList = "SYSTEM | 200 | Online Users: ";

    // Use the UserManager object's method to get the online users
    std::vector<std::string> onlineUsers = userManager.getOnlineUsernames();

    for (const auto &username : onlineUsers)
    {
        userList += username + ", ";
    }

    // Remove the trailing comma and space
    userList = userList.substr(0, userList.length() - 2);

    send(clientSocket, userList.c_str(), userList.size(), 0);
}