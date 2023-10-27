#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using std::cout;
using std::endl;

int main()
{
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    cout << "Socket " << serverSocket << " created." << endl;

    // Set up server address information
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Listen for incoming connections
    listen(serverSocket, 10);
    cout << "Server is listening on port 8080..." << endl;

    while (true)
    {
        // Accept a client connection
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

        cout << "Received connection request from " << inet_ntoa(clientAddr.sin_addr) << ":" << clientSocket << endl;

        // Grant the request and send a success message
        send(clientSocket, "SYSTEM | 200 | Successful connection.", 38, 0);
        cout << "Request granted." << endl;
        cout << "SYSTEM | 200 | Successful connection." << endl;

        // Close the client socket
        close(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
