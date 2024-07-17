#include "../../includes/Server.hpp"

void Server::partAllChannels(Client& client) {
	int clientSocket = client.getClientSocket();

	std::vector<std::string> channelsToRemove;

	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		Channel& channel = it->second;

		if (channel.isOperator(clientSocket))
			channel.removeOperator(clientSocket);
		if (channel.isInvitedMember(clientSocket))
			channel.removeInvitedMember(clientSocket);
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

void Server::partCmdClientIRSSI(Client& client, std::vector<std::string> tokens) {
	std::string reason = "Left the channel";
	std::string channelName = trim(tokens[1]);

	if (tokens.size() > 1)
		reason = joinTokens(tokens, 1);

	partCmdClientCommon(client, channelName, reason);
}

void Server::partCmdClientNC(Client& client, std::vector<std::string> tokens) {
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

	partCmdClientCommon(client, channelName, reason);
}

void Server::partCmdClientCommon(Client& client, const std::string& channelName, const std::string& reason) {
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end() || !it->second.isMember(client.getClientSocket())) {
		sendErrorMessage(client, std::string(RED) + "Error: You are not a member of the channel " + channelName + "\n" + RESET, 
						 ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@localhost NOTICE " + client.getUser().getNickname() + " :You are not a member of the channel " + channelName + "\r\n");
		return;
	}

	it->second.removeMember(client.getClientSocket());

	std::string leaveMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + std::string(CYAN_IRSSI) + client.getUser().getNickname() + std::string(RESET) + " [" + std::string(CYAN_IRSSI) + client.getUser().getUsername() + "@localhost" + std::string(RESET) + "]" + " has left " + std::string(BOLD) + channelName + std::string(RESET) + " [" + reason + "]\r\n";
	std::string leaveMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@localhost PART " + channelName + " :" + reason + "\r\n";

	const std::vector<int>& members = it->second.getMembers();
	for (std::vector<int>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		int memberSocket = *memberIt;
		Client& memberClient = _clients[memberSocket];
		std::string fullMessage = memberClient.isIrssi ? leaveMsgIrssi : leaveMsgNetcat;
		::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
	}

	if (client.isIrssi) {
		client.sendClientMsg(client.getClientSocket(), leaveMsgIrssi.c_str());
		std::string endOfNamesMsg = RPL_ENDOFNAMES(client.getUser().getNickname(), channelName);
		client.sendClientMsg(client.getClientSocket(), endOfNamesMsg.c_str());
	}

	std::stringstream ss;
	ss << GREEN << MSG_LEFT_CHAN << channelName << RESET << std::endl << std::endl;
	std::string channelLeavedMsg = ss.str();
	client.sendClientMsg(client.getClientSocket(), channelLeavedMsg.c_str());

	if (!it->second.isMember(client.getClientSocket()) && it->second.getMembersCount() == 0) {
		_channels.erase(it);
		_channelOrder.erase(std::remove(_channelOrder.begin(), _channelOrder.end(), channelName), _channelOrder.end());
		std::cout << BOLD << "Channel: [" << channelName << "] destroyed successfully! ❌" << RESET << std::endl;
	}
}

void Server::partCmdClient(Client& client, std::vector<std::string> tokens) {
	if (tokens.size() <= 1) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_LEAVE);
		return;
	}

	if (client.isIrssi)
		partCmdClientIRSSI(client, tokens);
	else
		partCmdClientNC(client, tokens);
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


// /connect localhost 6667 1
