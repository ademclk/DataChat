#ifndef CLIENTHANDLER_HPP
#define CLIENTHANDLER_HPP

#include "UserManager.hpp"
#include <string>
#include <vector>

/**
 * @class ClientHandler
 * @brief A class to handle client connections.
 *
 * This class handles client connections by reading and writing data to the client socket. It also manages a map of client usernames and a mutex for thread synchronization.
 */
class ClientHandler
{
public:
    ClientHandler(int clientSocket, UserManager &userManager);

    /**
     * @brief Handle the client connection.
     *
     * This function handles the client connection by reading and writing data to the client socket.
     */
    void handle();
    void sendHelpMessage();
    void listUsers();

private:
    int clientSocket;         // The client socket descriptor
    UserManager &userManager; // A reference to the UserManager object
    bool hasSetUsername;

    /**
     * @brief Handle client commands.
     *
     * This function processes client commands (e.g., !quit, !username) and performs the necessary actions.
     *
     * @param command The client command to handle.
     * @param username Reference to the current username of the client.
     */
    void handleCommand(const std::string &command, std::string &username);

    /**
     * @brief Handle regular messages.
     *
     * This function handles regular messages sent by clients and broadcasts them to all clients.
     *
     * @param message The regular message to handle.
     * @param username The current username of the client.
     */
    void handleRegularMessage(const std::string &message, const std::string &username);
};

#endif