#ifndef USERMANAGER_HPP
#define USERMANAGER_HPP

#include <map>
#include <mutex>

class UserManager
{
public:
    UserManager();
    ~UserManager();
    void updateUsernames(int clientSocket, const std::string &newUsername);
    std::string getUsername(int clientSocket) const;
    int getClientSocket(const std::string &username) const;
    void broadcastMessage(int senderSocket, const std::string &message);
    void removeClient(int clientSocket);
    std::vector<std::string> getOnlineUsernames() const;
    void sendPrivateMessage(int senderSocket, int recipientSocket, const std::string &message);

private:
    std::map<int, std::string> clientUsernames;
    mutable std::mutex mutex;
};

#endif