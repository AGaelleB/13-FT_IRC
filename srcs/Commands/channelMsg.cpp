#include "../../includes/Server.hpp"

void Server::broadcastMessageToChannel(const std::string& channelName, const std::string& message, int excludeSocket) {
	
	std::cout << YELLOW << "START broadcastMessageToChannel" << RESET << std::endl; 


    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        const std::vector<int>& members = it->second.getMembers();
        for (std::vector<int>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
            if (*memberIt != excludeSocket) {
                std::map<int, Client>::iterator clientIt = _clients.find(*memberIt);
                if (clientIt != _clients.end()) {
                    clientIt->second.sendClientMsg(clientIt->second.getClientSocket(), message.c_str());
                }
            }
        }
    }
}

void Server::ChannelMsg(Client& client) {

	std::cout << YELLOW << "START ChannelMsg" << RESET << std::endl; 

	char buffer[MAX_SIZE_MSG];
	int bytes_read = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
	if (bytes_read <= 0) {
		// Handle client disconnection
		close(client.getClientSocket());
		fds[client.getClientSocket()].fd = -1;
		_clients.erase(client.getClientSocket());
	}
	else {
		buffer[bytes_read] = '\0';
		std::string message(buffer);

		// Check if client is part of a channel and broadcast the message
		for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
			Channel& channel = it->second;
			if (channel.isMember(client.getClientSocket())) {
				std::string fullMessage = "<" + client.getUser().getNickname() + "> " + message;
				broadcastMessageToChannel(it->first, fullMessage, client.getClientSocket());
				break;
			}
		}
	}
}