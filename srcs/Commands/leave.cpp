#include "../../includes/Server.hpp"

void Server::leaveAllChannels(Client& client) {

    // std::cout << YELLOW << "leaveAllChannels START" << RESET << std::endl;

    int clientSocket = client.getClientSocket();

    std::vector<std::string> channelsToRemove;

    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        Channel& channel = it->second;

        if (channel.isMember(clientSocket)) {
            channel.removeMember(clientSocket);
            std::string leaveMsg = BOLD "<" + client.getUser().getNickname() + "> has left the channel " + it->first + "\n" RESET;
            broadcastMessageToChannel(it->first, leaveMsg, clientSocket);

            if (channel.getMembersCount() == 0)
                channelsToRemove.push_back(it->first);
        }
    }

    for (std::vector<std::string>::iterator it = channelsToRemove.begin(); it != channelsToRemove.end(); ++it) {
        _channels.erase(*it);
        _channelOrder.erase(std::remove(_channelOrder.begin(), _channelOrder.end(), *it), _channelOrder.end());
        std::cout << BOLD << "Channel: [" << *it << "] destroyed successfully! ❌" << RESET << std::endl;
    }
}

void Server::leaveChannelIRSSI(Client& client, std::vector<std::string> tokens) {
	std::string reason = "Left the channel";
	std::string channelName = trim(tokens[1]);

	if (tokens.size() > 1)
		reason = joinTokens(tokens, 1);
	leaveChannelCommon(client, channelName, reason);
}

void Server::leaveChannelNC(Client& client, std::vector<std::string> tokens) {
	std::string reason = "Left the channel";
	std::string channelName;

	if (tokens.size() == 2)
		channelName = trim(tokens[1]);
	else if (tokens.size() > 2) {
		channelName = trim(tokens[1]);
		reason = joinTokens(tokens, 2);
	}
	if (channelName[0] != '#')
		channelName = "#" + channelName;

	leaveChannelCommon(client, channelName, reason);
}

void Server::sendPartMessage(Client& client, Channel& channel, const std::string& reason) {
    std::string nick = client.getUser().getNickname();
    std::string user = client.getUser().getUsername();
    std::string toSend;

    for (size_t i = 0; i < channel.getMembers().size(); ++i) {
        int memberSocket = channel.getMembers()[i];
        Client& member = _clients[memberSocket];
        toSend.clear();

        if (member.isIrssi) {
            toSend = ":" + nick + "!" + user + "@hostname PART " + channel.getName() + " :" + reason + "\r\n";
        } else {
            toSend = "\e[0;36m -!- " + nick + " [" + user + "\e[0m@hostname] has left " + channel.getName() + " [" + reason + "]\r\n";
        }

        send(member.getClientSocket(), toSend.c_str(), toSend.size(), 0);
    }
}

void Server::leaveChannelCommon(Client& client, const std::string& channelName, const std::string& reason) {
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it != _channels.end() && it->second.isMember(client.getClientSocket())) {
        it->second.removeMember(client.getClientSocket());

        sendPartMessage(client, it->second, reason);

        if (client.isIrssi) {
            std::string endOfNamesMsg = RPL_ENDOFNAMES(client.getUser().getNickname(), channelName);
            client.sendClientMsg(client.getClientSocket(), endOfNamesMsg.c_str());
        }

        std::stringstream ss;
        ss << GREEN << MSG_LEFT_CHAN << channelName << RESET << std::endl << std::endl;
        std::string channelLeavedMsg = ss.str();
        client.sendClientMsg(client.getClientSocket(), channelLeavedMsg.c_str());

        if (it->second.getMembersCount() == 0) {
            _channels.erase(it);
            _channelOrder.erase(std::remove(_channelOrder.begin(), _channelOrder.end(), channelName), _channelOrder.end());
            std::cout << BOLD << "Channel: [" << channelName << "] destroyed successfully! ❌" << RESET << std::endl;
        }
    } else {
        client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_FAILED_LEAVE);
    }
}

void Server::leaveChannel(Client& client, std::vector<std::string> tokens) {
	if (tokens.size() <= 1) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_LEAVE);
		return;
	}

	if (client.isIrssi)
		leaveChannelIRSSI(client, tokens);
	else
		leaveChannelNC(client, tokens);
}

std::string Server::joinTokens(const std::vector<std::string>& tokens, size_t startIndex) {
	std::string result;
	for (size_t i = startIndex; i < tokens.size(); ++i) {
		if (i != startIndex)
			result += " ";
		result += tokens[i];
	}
	return (result);
}

/* 

	leave ne fonctionne pas avec irssi

	/connect localhost 6667 1
	/join server
	/leave #server
	/part #server
	/part


 */