#ifndef SOCKETUTILS_HPP
#define SOCKETUTILS_HPP

#include <string>

void sendDelimitedMessage(int socket, const std::string &message);
std::string receiveDelimitedMessage(int socket);

#endif
