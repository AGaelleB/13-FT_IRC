#include "../../includes/Server.hpp"

void Server::leaveAllChannels(Client& client) {
	int clientSocket = client.getClientSocket();
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		Channel& channel = it->second;
		if (channel.isMember(clientSocket)) {
			channel.removeMember(clientSocket);
			std::string leaveMsg = BOLD "<" + client.getUser().getNickname() + "> has left the channel " + it->first + "\n" RESET;
			broadcastMessageToChannel(it->first, leaveMsg, clientSocket);
		}
	}
}

void Server::leaveChannel(Client& client, std::vector<std::string> tokens) {
	
	if (tokens.size() != 2) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_LEAVE);
		return;
	}

	std::string channelName = trim(tokens[1]);

	if (channelName[0] != '#')
		channelName = "#" + channelName;

	std::cout << YELLOW << "BEFORE channelName =" << channelName << RESET << std::endl;

	// Remove trailing colon if it exists
	if (!channelName.empty() && channelName[channelName.size() - 1] == ':') {
		channelName.erase(channelName.size() - 1);
	}

	std::cout << YELLOW << "AFTER channelName =" << channelName << RESET << std::endl;

	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it != _channels.end() && it->second.isMember(client.getClientSocket()))
		it->second.removeMember(client.getClientSocket());
	else {
		std::cout << YELLOW << "FAILED !!!!!!!!!!!1" << RESET << std::endl;
		client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_FAILED_LEAVE);
		return;
	}

	std::cout << YELLOW << "END channelName =" << channelName << RESET << std::endl;

	std::stringstream ss;
	ss << GREEN "You have leave the channel " << channelName << RESET << std::endl << std::endl;
	std::string channelLeavedMsg = ss.str();
	client.sendClientMsg(client.getClientSocket(), channelLeavedMsg.c_str());

	std::string joinMsg = BOLD "<" + client.getUser().getNickname() + "> has left the channel " + channelName + "\n" RESET;
	broadcastMessageToChannel(channelName, joinMsg, client.getClientSocket());
	
}


/* 

	leave ne fonctionne pas avec irssi

	/connect localhost 6667 1
	/join server
	/leave #server

 */