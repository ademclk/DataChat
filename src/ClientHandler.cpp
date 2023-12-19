#include "ClientHandler.hpp"
#include "SocketUtils.hpp"
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
    std::string connectionMessageText = "Welcome to the chat! Please set your username using !username <your_username>.";
    Message connectionMessage(connectionMessageText, "SYSTEM", CommandType::MESG);
    std::string connectionMessageStr = connectionMessage.getFormattedMessage();
    sendDelimitedMessage(clientSocket, connectionMessageStr);

    // Buffer to store the received messages
    char buffer[4096];

    // Default username for the client
    std::string username = "Guest";

    while (!hasSetUsername)
    {
        // Receive message from the client
        std::string clientMessage = receiveDelimitedMessage(clientSocket);
        if (clientMessage.empty())
        {
            std::cout << "Client " << username << " disconnected." << std::endl;
            break;
        }

        // Convert the received message to a string
        Message receivedMessage = Message::parseFromString(clientMessage);

        if (receivedMessage.getContent().substr(0, 9) == "!username")
        {
            std::string newUsername = receivedMessage.getContent().substr(10);
            username = newUsername;
            userManager.updateUsernames(clientSocket, username);
            hasSetUsername = true;

            // Notify the original sender client about the username change
            std::string message = "Username set to " + username + ". You can now start chatting!";
            Message successMessage(message, "SYSTEM", CommandType::MESG);
            std::string successMessageStr = successMessage.getFormattedMessage();
            sendDelimitedMessage(clientSocket, successMessageStr);

            // Broadcast to all clients (excluding the original sender) that the user has set a username
            std::string broadcastMessage = "User " + username + " has joined the chat!";
            userManager.broadcastMessage(clientSocket, broadcastMessage);

            // Send help message
            sendHelpMessage();
        }
        else
        {
            // Inform the client to set a username
            Message setUsernameMessage("Set your username using !username <username>", "SYSTEM", CommandType::MESG);
            std::string messageStr = setUsernameMessage.getFormattedMessage();
            sendDelimitedMessage(clientSocket, messageStr);
        }
    }

    while (true)
    {
        std::string clientMessage = receiveDelimitedMessage(clientSocket);
        if (clientMessage.empty())
        {
            std::cout << "Client " << username << " disconnected." << std::endl;
            break;
        }

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
            sendDelimitedMessage(clientSocket, errorMessage);
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
    std::string helpMessageText = "\nSYSTEM | 200 | Available Commands:\n"
                                  "!help                - Display this help message\n"
                                  "!username <new_name> - Change your username\n"
                                  "!list                - List of online users\n"
                                  "!quit                - Quit the chat\n";

    Message helpMessage(helpMessageText, "SYSTEM", CommandType::MESG);

    std::string helpMessageStr = helpMessage.getFormattedMessage();
    sendDelimitedMessage(clientSocket, helpMessageStr);
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

    sendDelimitedMessage(clientSocket, userList);
}