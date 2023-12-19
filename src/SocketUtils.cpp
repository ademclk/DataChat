#include "SocketUtils.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

void sendDelimitedMessage(int socket, const std::string &message)
{
    uint32_t length = htonl(message.size()); // convert size to network byte order

    ::send(socket, &length, sizeof(length), 0);         // send the length prefix
    ::send(socket, message.c_str(), message.size(), 0); // send the message
}

std::string receiveDelimitedMessage(int socket)
{
    uint32_t length;
    ::recv(socket, &length, sizeof(length), 0); // receive the length prefix
    length = ntohl(length);                     // convert length to host byte order

    char buffer[length + 1];           // buffer to store the received message
    ::recv(socket, buffer, length, 0); // receive the message

    buffer[length] = '\0';      // null-terminate the message
    return std::string(buffer); // convert to a string
}
