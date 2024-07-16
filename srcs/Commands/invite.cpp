#include "../../includes/Server.hpp"

void Server::sendErrorMessage(Client& client, const std::string& errorMsgNetcat, const std::string& errorMsgIrssi) {
	if (client.isIrssi)
		::send(client.getClientSocket(), errorMsgIrssi.c_str(), errorMsgIrssi.size(), 0);
	else
		::send(client.getClientSocket(), errorMsgNetcat.c_str(), errorMsgNetcat.size(), 0);
}

int Server::findUserSocket(const std::string& nickname) {
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.getUser().getNickname() == nickname) {
			return it->second.getClientSocket();
		}
	}
	return (-1);
}

bool Server::validateInviteCommand(Client& client, const std::vector<std::string>& tokens) {
	if (tokens.size() != 3) {
		std::string errorMsgNetcat = std::string(RED) + "Error: Invalid number of parameters. Usage: /INVITE <user> <channel>\n" + RESET;
		std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :Invalid number of parameters. Usage: /INVITE <user> <channel>\r\n";
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return (false);
	}
	return (true);
}

bool Server::validateChannelMembership(Client& client, const std::string& channelName) {
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end()) {
		std::string errorMsgNetcat = std::string(RED) + "Error: Channel " + channelName + " not found\n" + RESET;
		std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :Channel " + channelName + " not found\r\n";
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return (false);
	}

	Channel& channel = it->second;
	if (!channel.isMember(client.getClientSocket())) {
		std::string errorMsgNetcat = std::string(RED) + "Error: You are not a member of the channel " + channelName + "\n" + RESET;
		std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :You are not a member of the channel " + channelName + "\r\n";
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return (false);
	}
	return (true);
}

void Server::sendInviteMessages(Client& client, Channel& channel, const std::string& channelName, Client& invitedClient, int userSocket) {

	std::string inviteMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + client.getUser().getNickname() + " invited " + invitedClient.getUser().getNickname() + " to join " + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "\r\n";
	std::string inviteMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname INVITE " + invitedClient.getUser().getNickname() + " :" + channelName + "\r\n";

	const std::vector<int>& members = channel.getMembers();
	for (size_t i = 0; i < members.size(); ++i) {
		int memberSocket = members[i];
		Client& memberClient = _clients[memberSocket];

		std::string fullMessage;
		if (memberClient.isIrssi)
			fullMessage = inviteMsgIrssi;
		else
			fullMessage = inviteMsgNetcat;
		::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
	}

	if (invitedClient.isIrssi) {
		std::string invitedMessageIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + invitedClient.getUser().getNickname() + " :You have been invited to join " + channelName + " by " + client.getUser().getNickname() + "\r\n";
		::send(userSocket, invitedMessageIrssi.c_str(), invitedMessageIrssi.size(), 0);
	}
	else {
		std::string invitedMessageNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + std::string(CYAN_IRSSI) + invitedClient.getUser().getNickname() + std::string(RESET) + " [" + std::string(CYAN_IRSSI) + client.getUser().getUsername() + "@localhost" + std::string(RESET) + "]" + " has been invited to join " + std::string(BOLD) + channelName + std::string(RESET) + " by " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "\r\n";
		::send(userSocket, invitedMessageNetcat.c_str(), invitedMessageNetcat.size(), 0);
	}
}

void Server::inviteCmdClient(Client& client, std::vector<std::string> tokens) {
	if (!validateInviteCommand(client, tokens))
		return;

	std::string channelName = tokens[2];
	if (channelName[0] != '#')
		channelName = "#" + channelName;

	if (!validateChannelMembership(client, channelName))
		return;

	int userSocket = findUserSocket(tokens[1]);
	if (userSocket == -1) {
		std::string errorMsgNetcat = std::string(RED) + "Error: User " + tokens[1] + " not found\n" + RESET;
		std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :User " + tokens[1] + " not found\r\n";
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi); 
		return;
	}

	Client& invitedClient = _clients[userSocket];
	Channel& channel = _channels[channelName];

	if (channel.isMember(userSocket)) {
		std::string errorMsgNetcat = std::string(RED) + "Error: User " + invitedClient.getUser().getNickname() + " is already in the channel " + channelName + "\n" + RESET;
		std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :User " + invitedClient.getUser().getNickname() + " is already in the channel " + channelName + "\r\n";
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return;
	}

	channel.addInvitedMember(userSocket);
	sendInviteMessages(client, channel, channelName, invitedClient, userSocket);
}

void Channel::addInvitedMember(int clientToAdd) {
	if (!isInvitedMember(clientToAdd))
		_membersIsInvite.push_back(clientToAdd);
}

void	Channel::removeInvitedMember(int clientToErase) {
	std::vector<int>::iterator it;
	for (it = _membersIsInvite.begin(); it != _membersIsInvite.end(); ++it) {
		if (*it == clientToErase) {
			_membersIsInvite.erase(it);
			break; 
		}
	}
}

bool Channel::isInvitedMember(int clientSocket) const {
	std::vector<int>::const_iterator it;
	for (it = _membersIsInvite.begin(); it != _membersIsInvite.end(); ++it) {
		if (*it == clientSocket)
			return (true);
	}
	return (false);
}

// /connect localhost 6667 1
