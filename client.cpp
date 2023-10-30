#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <mutex>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::lock_guard;
using std::mutex;
using std::string;
using std::thread;

// Mutex for thread-safe printing
mutex displayMutex;

// Function to receive and display messages
void receiveMessages(int clientSocket)
{
    char buffer[4096];
    while (true)
    {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            cout << "Disconnected from server." << endl;
            break;
        }
        buffer[bytesRead] = '\0';

        // Lock the display while printing
        lock_guard<mutex> lock(displayMutex);

        cout << buffer << endl;
    }
}

int main()
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    cout << "Socket " << clientSocket << " created." << endl;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

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

    // Start a thread to receive and display messages
    thread receiveThread(receiveMessages, clientSocket);

    while (true)
    {
        string message;
        getline(cin, message);

        if (message.substr(0, 1) == "!")
        {
            if (message == "!quit")
            {
                cout << "Quitting..." << endl;
                send(clientSocket, message.c_str(), message.size(), 0);
                break;
            }
            else if (message.substr(0, 9) == "!username")
            {
                string oldUsername = username;
                username = message.substr(9);
                cout << "Updated username to " << username << "." << endl;
                send(clientSocket, message.c_str(), message.size(), 0);
            }
            // Implement other client commands as needed
        }
        else
        {
            string messageWithUsername = username + ": " + message;

            // Lock the display while printing
            lock_guard<mutex> lock(displayMutex);

            cout << messageWithUsername << endl;
            send(clientSocket, messageWithUsername.c_str(), messageWithUsername.size(), 0);
        }
    }

    // Close the client socket
    close(clientSocket);

    return 0;
}
