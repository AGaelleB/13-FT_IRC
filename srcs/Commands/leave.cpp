#include "../../includes/Server.hpp"

void Server::leaveAllChannels(Client& client) {
	std::cout << YELLOW << "leaveAllChannels START" << RESET << std::endl;

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

    // Remove trailing colon if it exists
    if (!channelName.empty() && channelName[channelName.size() - 1] == ':') {
        channelName.erase(channelName.size() - 1);
    }

    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it != _channels.end() && it->second.isMember(client.getClientSocket())) {
        it->second.removeMember(client.getClientSocket());

		if (client.isIrssi) {
			// Notify other members about the client leaving
			std::string leaveMsg = RPL_PART(client.getUser().getNickname(), client.getUser().getUsername(), channelName, "Left the channel");
			broadcastMessageToChannel(channelName, leaveMsg, client.getClientSocket());

			// Send the PART message to the client itself
			client.sendClientMsg(client.getClientSocket(), leaveMsg.c_str());

			// Send the end of names list message to ensure Irssi updates properly
			std::string endOfNamesMsg = RPL_ENDOFNAMES(client.getUser().getNickname(), channelName);
			client.sendClientMsg(client.getClientSocket(), endOfNamesMsg.c_str());
		}


        // // If the channel is empty after the client leaves, remove the channel
        // if (it->second.getMembers().empty()) {
        //     _channels.erase(it);
        // }

    }
	else {
        client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_FAILED_LEAVE);
        return;
    }

    // Send confirmation to the client
    std::stringstream ss;
    ss << GREEN << "You have left the channel " << channelName << RESET << std::endl << std::endl;
    std::string channelLeavedMsg = ss.str();
    client.sendClientMsg(client.getClientSocket(), channelLeavedMsg.c_str());
}


/* 

	leave ne fonctionne pas avec irssi

	/connect localhost 6667 1
	/join server
	/leave #server
	/part #server
	/part


 */