#include "../../includes/Server.hpp"

void Server::privMsgCmdClient(Client& client, const std::string& target, const std::string& message) {
    int recipientSocket = -1;

	std::string trim_msg = trim(message);

    // Find the target client by nickname
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getUser().getNickname() == target) {
            recipientSocket = it->first; // Get the socket from the map key
            break;
        }
    }

    // If the target client is not found, send an error to the sender
    if (recipientSocket == -1) {
        std::string error = ERR_NOSUCHNICK(client.getUser().getNickname(), target);
        client.sendClientMsg(client.getClientSocket(), error.c_str());
        return;
    }

    // Construct the message to be sent
    std::string fullMessage = RPL_PRIVMSG(client.getUser().getNickname(), client.getUser().getUsername(), target, trim_msg);

    // Send the message to the target client
    _clients[recipientSocket].sendClientMsg(recipientSocket, fullMessage.c_str());

    // Log the private message on the server side for debugging purposes
    std::cout << "Private message from " << client.getUser().getNickname() << " to " << target << ": " << trim_msg << std::endl;
}
