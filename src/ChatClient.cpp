#include "ChatClient.hpp"
#include "SocketUtils.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <mutex>
#include <fstream>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>

ChatClient::ChatClient() : clientSocket(-1), username("Guest"), shouldContinue(true)
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
    while (shouldContinue)
    {
        std::string message = receiveDelimitedMessage(clientSocket);
        if (message.empty())
        {
            std::cerr << "Failed to receive data: " << strerror(errno) << std::endl;
            break;
        }

        try
        {
            clearLine();
            Message parsedMessage = Message::parseFromString(message);
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::cout << parsedMessage.getFormattedMessage() << std::endl;
            writeToLogFile(parsedMessage.getFormattedMessage());
        }
        catch (std::runtime_error &err)
        {
            // If an error occurs while parsing the message, it means that the message has been corrupted
            std::cout << "Message was corrupted. Sending MERR..." << std::endl;

            // Send a MERR command to the server
            Message errorMessage("Message error", username, CommandType::MERR);
            std::string errorMessageStr = errorMessage.getFormattedMessage();
            writeToLogFile(errorMessageStr);
            sendDelimitedMessage(clientSocket, errorMessageStr);
        }
    }
}

void ChatClient::handleUserInput()
{
    while (shouldContinue)
    {
        std::string input;
        getline(std::cin, input);

        if (input.empty())
        {
            clearLine();
            std::cout << "\x1B[A"; // Move up one line
            std::cout << "\x1B[K"; // Clear the line
            continue;
        }

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
        Message quitMessage(command, username, CommandType::GONE);

        sendMessage(quitMessage);
        shouldContinue = false;

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
        Message privateMessage(command, username, CommandType::MESG);
        std::cout << privateMessage.getFormattedMessage() << std::endl;
        sendMessage(privateMessage);
    }
}

void ChatClient::updateUsername(const std::string &newUsername)
{
    std::string oldUsername = username;
    username = newUsername;

    // Get current date and time
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::string date = std::to_string(1900 + ltm->tm_year) + "-" +
                       std::to_string(1 + ltm->tm_mon) + "-" +
                       std::to_string(ltm->tm_mday) + "_" +
                       std::to_string(1 + ltm->tm_hour) + "-" +
                       std::to_string(1 + ltm->tm_min) + "-" +
                       std::to_string(1 + ltm->tm_sec);

    // Format the new filename using the current date, time, and new username
    std::string newLogFilename = "logs/" + date + "_" + username + ".txt";

    // Rename the log file
    std::rename(logFilename.c_str(), newLogFilename.c_str());

    // Update logFilename to the new filename
    logFilename = newLogFilename;

    Message updateUsernameMessage("!username " + newUsername, username, CommandType::MESG);
    sendMessage(updateUsernameMessage);
}

void ChatClient::sendMessage(const Message &message)
{
    clearLine();
    std::cout << "\x1B[A"; // Move up one line
    std::cout << "\x1B[K"; // Clear the line
    std::string messageStr = message.getFormattedMessage();
    writeToLogFile(messageStr);
    sendDelimitedMessage(clientSocket, messageStr);
}

void ChatClient::writeToLogFile(const std::string &message)
{
    struct stat st = {0};
    if (stat("logs", &st) == -1)
    {
        mkdir("logs", 0700);
    }

    // Open the log file and append the message
    std::ofstream logFile;
    logFile.open(logFilename.c_str(), std::ios_base::app);
    if (logFile.is_open())
    {
        logFile << message << std::endl;
        logFile.close();
    }
    else
        std::cout << "Unable to open file";
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

    // Get current date and time
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::string date = std::to_string(1900 + ltm->tm_year) + "-" +
                       std::to_string(1 + ltm->tm_mon) + "-" +
                       std::to_string(ltm->tm_mday) + "_" +
                       std::to_string(1 + ltm->tm_hour) + "-" +
                       std::to_string(1 + ltm->tm_min) + "-" +
                       std::to_string(1 + ltm->tm_sec);

    // Format the filename using the current date, time, and username
    logFilename = "logs/" + date + ".txt";

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
