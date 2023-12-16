#include "ServerSocket.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

/**
 * @brief Construct a new ServerSocket object.
 *
 * This constructor creates a new ServerSocket object, initializes the server socket, binds it to the specified address and port, and starts listening for incoming connections.
 */
ServerSocket::ServerSocket()
{
    // Create a new socket and store its descriptor
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }
    std::cout << "Socket " << serverSocket << " created." << std::endl;

    // Define the server address structure
    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_family = AF_INET;

    // Enable address reuse option
    int addressReuseOption = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &addressReuseOption, sizeof(addressReuseOption)) == -1)
    {
        close(serverSocket);
        throw std::runtime_error("Failed to set socket options: " + std::string(strerror(errno)));
    }

    // Bind the server socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        close(serverSocket);
        throw std::runtime_error("Failed to bind to port " + std::to_string(SERVER_PORT) + ": " + std::string(strerror(errno)));
    }

    // Start listening for incoming connections
    if (listen(serverSocket, MAX_INCOMING_CONNECTIONS) == -1)
    {
        close(serverSocket);
        throw std::runtime_error("Failed to listen on port " + std::to_string(SERVER_PORT) + ": " + std::string(strerror(errno)));
    }

    if (serverAddr.sin_addr.s_addr == INADDR_NONE)
    {
        close(serverSocket);
        throw std::runtime_error("Invalid address: " + std::string(SERVER_ADDRESS));
    }

    std::cout << "Server is listening on port " << SERVER_PORT << "..." << std::endl;
}

/**
 * @brief Destroy the ServerSocket object.
 *
 * This destructor closes the server socket.
 */
ServerSocket::~ServerSocket()
{
    // Close the server socket
    close(serverSocket);
}

/**
 * @brief Get the server socket descriptor.
 *
 * This function returns the server socket descriptor.
 *
 * @return int | The server socket descriptor.
 */
int ServerSocket::getSocket() const
{
    // Return the server socket descriptor
    return serverSocket;
}

int ServerSocket::acceptConnection() const
{
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == -1)
    {
        throw std::runtime_error("Failed to accept connection: " + std::string(strerror(errno)));
    }
    return clientSocket;
}

void ServerSocket::shutdownServer() const
{
    if (shutdown(serverSocket, SHUT_RDWR) == -1)
    {
        std::cerr << "Failed to shutdown server socket: " << strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "Server socket has been shut down." << std::endl;
    }
}