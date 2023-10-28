#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <vector>
#include <unistd.h>

using std::cout;
using std::endl;
using std::thread;
using std::vector;

void handleClient(int clientSocket)
{
    send(clientSocket, "SYSTEM | 200 | Welcome to the server!", 38, 0);

    // Close the client socket
    close(clientSocket);
}

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

    vector<thread> clientThreads;

    while (true)
    {
        // Accept a client connection
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

        // Create a new thread to handle the client
        cout << "Received connection request from " << inet_ntoa(clientAddr.sin_addr) << ":" << clientSocket << endl;

        clientThreads.emplace_back(handleClient, clientSocket);
        clientThreads.back().detach(); // Detach the thread
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
