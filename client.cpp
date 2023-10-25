#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

int main()
{
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Set up server address information
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);                   // Port number
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server's IP address

    // Connect to the server
    connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    std::cout << "Connecting to server..." << std::endl;

    char buffer[4096];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0)
    {
        std::cerr << "Failed to receive response from server." << std::endl;
    }
    else
    {
        buffer[bytesRead] = '\0';
        std::cout << buffer << std::endl;
        std::cout << "You've successfully connected." << std::endl;
    }

    // Close the client socket
    close(clientSocket);

    return 0;
}
