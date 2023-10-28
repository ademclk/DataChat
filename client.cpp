#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

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

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Connection failed");
        close(clientSocket);
        return -1;
    }

    char buffer[4096];
    string username;
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    cout << "Connecting to server..." << endl;
    cout << buffer << endl;

    while (true)
    {
        cout << username << "> ";
        string message;
        getline(cin, message);

        if (message.substr(0, 1) == "!")
        {
            // Handle client commands
            if (message == "!quit")
            {
                cout << "Quitting..." << endl;
                break;
            }
            else if (message.substr(0, 9) == "!username")
            {
                username = message.substr(9);
                cout << "Updated username to " << username << "." << endl;
                send(clientSocket, message.c_str(), message.size(), 0); // Send username change request to the server
            }
            // Implement other client commands as needed
        }
        else
        {
            // Send a regular message to the server
            send(clientSocket, message.c_str(), message.size(), 0);
        }
    }

    // Close the client socket
    close(clientSocket);

    return 0;
}
