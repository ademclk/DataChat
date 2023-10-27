#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

using std::cerr;
using std::cout;
using std::endl;

int main()
{
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    cout << "Socket " << clientSocket << " created." << endl;

    // Set up server address information
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);                   // Port number
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server's IP address

    // Connect to the server
    cout << "Connecting to server..." << endl;
    connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    char buffer[4096];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0)
    {
        cerr << "Failed to receive response from server." << endl;
    }
    else
    {
        buffer[bytesRead] = '\0';
        cout << buffer << endl;
        cout << "You've successfully connected." << endl;
    }

    // Close the client socket
    close(clientSocket);

    return 0;
}
