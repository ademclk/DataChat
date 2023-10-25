#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Socket " << serverSocket << " created." << std::endl;

    // Set up server address information
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Listen for incoming connections
    listen(serverSocket, 10);

    std::cout << "Server is listening on port 8080..." << std::endl;

    while (true)
    {
        // Accept a client connection
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

        std::cout << "Received connection request from " << inet_ntoa(clientAddr.sin_addr) << ":" << clientSocket << std::endl;

        // Grant the request and send a success message
        send(clientSocket, "SYSTEM | 200 | Successful connection.", 38, 0);
        std::cout << "Request granted." << std::endl;
        std::cout << "SYSTEM | 200 | Successful connection." << std::endl;

        // Close the client socket
        close(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
