#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <vector>
#include <unistd.h>
#include <map>

// Instead, using namespace std;
using std::cout;
using std::endl;
using std::map;
using std::string;
using std::thread;
using std::vector;

// Store usernames of connected clients
map<int, string> clientUsernames;

void handleClient(int clientSocket)
{
    send(clientSocket, "SYSTEM | 200 | Successful connection.", 38, 0);

    char buffer[4096];
    string username = "Guest";

    while (true)
    {
        // Receive message from client
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived < 0)
        {
            cout << "Client " << username << " disconnected." << endl;
            break;
        }

        buffer[bytesReceived] = '\0';

        string clientMessage = buffer;

        // Handle client commands
        if (clientMessage.substr(0, 1) == "!")
        {
            if (clientMessage == "!quit")
            {
                string quitMessage = "SYSTEM | 200 | " + username + " has quit.";
                cout << quitMessage << endl;
                for (const auto &[client, clientUsername] : clientUsernames)
                {
                    if (client != clientSocket)
                    {
                        send(client, quitMessage.c_str(), quitMessage.size(), 0);
                    }
                }
                break;
            }
            else if (clientMessage.substr(0, 9) == "!username")
            {
                string oldUsername = username;
                username = clientMessage.substr(9);
                clientUsernames[clientSocket] = username;
                string usernameMessage = "SYSTEM | 200 | " + oldUsername + " updated username to " + username + ".";
                cout << usernameMessage << endl;
                for (const auto &[client, clientUsername] : clientUsernames)
                {
                    if (client != clientSocket)
                    {
                        send(client, usernameMessage.c_str(), usernameMessage.size(), 0);
                    }
                }
            }
            else
            {
                cout << "Received mesage from " << username << ": " << clientMessage << endl;
                for (const auto &[client, clientUsername] : clientUsernames)
                {
                    if (client != clientSocket)
                    {
                        send(client, (clientUsername + ": " + clientMessage).c_str(), clientMessage.size() + clientUsername.size() + 2, 0);
                    }
                }
            }
        }
    }

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

        thread clientThread(handleClient, clientSocket);
        clientThread.detach(); // Detach the thread
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
