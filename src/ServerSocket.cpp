#include "ServerSocket.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * @brief Construct a new ServerSocket object.
 *
 * This constructor creates a new ServerSocket object, initializes the server socket, binds it to the specified address and port, and starts listening for incoming connections.
 */
ServerSocket::ServerSocket()
{
    // Create a new socket and store its descriptor
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Socket " << serverSocket << " created." << std::endl;

    // Define the server address structure
    sockaddr_in serverAddr;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_family = AF_INET;

    // Bind the server socket to the specified address and port
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Start listening for incoming connections
    listen(serverSocket, MAX_INCOMING_CONNECTIONS);
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