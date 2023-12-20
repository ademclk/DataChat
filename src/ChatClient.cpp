#include "ChatClient.hpp"
#include "SocketUtils.hpp"
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
    while (true)
    {
        std::string message = receiveDelimitedMessage(clientSocket);
        if (message.empty())
        {
            std::cerr << "Failed to receive data: " << strerror(errno) << std::endl;
            break;
        }

        clearLine();
        Message parsedMessage = Message::parseFromString(message);
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << parsedMessage.getFormattedMessage() << std::endl;
    }
}

void ChatClient::handleUserInput()
{
    while (true)
    {
        std::string input;
        getline(std::cin, input);

        if (input.substr(0, 1) == "!")
        {
            std::lock_guard<std::mutex> lock(consoleMutex);
            handleClientCommand(input);
        }
        else
        {
            std::lock_guard<std::mutex> lock(consoleMutex);
            Message message(input, username, CommandType::MESG);
            sendMessage(message);
        }
    }
}

void ChatClient::handleClientCommand(const std::string &command)
{
    if (command == "!quit")
    {
        std::cout << "Quitting..." << std::endl;
        Message quitMessage("", username, CommandType::GONE);
        sendMessage(quitMessage);
        closeConnection();
        return;
    }

    if (command.substr(0, 9) == "!username")
    {
        updateUsername(command.substr(10));
    }

    if (command == "!help" || command == "!list")
    {
        Message commandMessage(command, username, CommandType::MESG);
        sendMessage(commandMessage);
    }

    if (command.substr(0, 8) == "!private")
    {
        std::string recipientAndMsg = command.substr(9);
        size_t separator = recipientAndMsg.find(' ');
        std::string recipient = recipientAndMsg.substr(0, separator);
        std::string msg = recipientAndMsg.substr(separator + 1);
        Message message(recipient + "|" + msg, username, CommandType::MESG);
        sendMessage(message);
    }
}

void ChatClient::updateUsername(const std::string &newUsername)
{
    std::string oldUsername = username;
    username = newUsername;
    Message updateUsernameMessage("!username " + newUsername, username, CommandType::MESG);
    sendMessage(updateUsernameMessage);
}

void ChatClient::sendMessage(const Message &message)
{
    clearLine();
    std::cout << "\x1B[A"; // Move up one line
    std::cout << "\x1B[K"; // Clear the line
    std::string messageStr = message.getFormattedMessage();
    sendDelimitedMessage(clientSocket, messageStr);
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

    std::cout << "Connecting to server..." << std::endl;
    std::string serverMessage = receiveDelimitedMessage(clientSocket);
    std::cout << serverMessage << std::endl;

    std::thread receiveThread(&ChatClient::receiveMessages, this);
    std::thread userInputThread(&ChatClient::handleUserInput, this);

    receiveThread.join(); // Wait for the receive thread to finish
    userInputThread.join();

    closeConnection();
}

void ChatClient::closeConnection()
{
    close(clientSocket);
}
