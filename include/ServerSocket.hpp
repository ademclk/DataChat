#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

// Define server address and port
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8080

// Define the maximum number of incoming connections that server can queue
#define MAX_INCOMING_CONNECTIONS 10

/**
 * @class ServerSocket
 * @brief A class to encapsulate a server socket.
 *
 * This class encapsulates a server socket, which is used to listen for incoming client connections.
 */
class ServerSocket
{
public:
    /**
     * @brief Construct a new ServerSocket object.
     *
     * This constructor creates a new ServerSocket object, initializes the server socket, binds it to the specified address and port, and starts listening for incoming connections.
     */
    ServerSocket();

    /**
     * @brief Destroy the ServerSocket object.
     *
     * This destructor closes the server socket.
     */
    ~ServerSocket();

    /**
     * @brief Get the server socket descriptor.
     *
     * This function returns the server socket descriptor.
     *
     * @return int | The server socket descriptor.
     */
    int getSocket() const;

private:
    // The server socket descriptor
    int serverSocket;
};

#endif