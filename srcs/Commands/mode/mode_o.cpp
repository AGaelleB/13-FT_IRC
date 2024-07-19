#include "../../../includes/Server.hpp"

/* 

+o donne les privilèges des opérateurs :
	Changer le topic du canal.
	Expulser des utilisateurs du canal.
	Définir d'autres modes de canal.
	Attribuer ou révoquer le statut d'opérateur à d'autres utilisateurs.
	
 */

void Server::modeOCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
	if (tokens.size() != 4) {
		std::string netcatMessage = "Error: Must be: /MODE <channel> <+o / -o> <user nickname>\r\n";
		std::string irssiMessage = ":localhost 461 " + client.getUser().getNickname() + " MODE_O :Not enough parameters\r\n";
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}

	if (tokens[2] == "+o")
		modePlusOCmd(client, tokens, channel, channelName);
	else if (tokens[2] == "-o")
		modeMinusdOCmd(client, tokens, channel, channelName);
}

void Server::modePlusOCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
	int userSocket = -1;
	Client* memberClient = NULL;
	std::string fullMessage;
	for (std::vector<int>::const_iterator it = channel.getMembers().begin(); it != channel.getMembers().end(); ++it) {
		Client& tempClient = getClientBySocket(*it);
		if (tempClient.getUser().getNickname() == tokens[3]) {
			userSocket = tempClient.getClientSocket();
			memberClient = &tempClient;
			break;
		}
	}
	if (userSocket != -1 && memberClient != NULL) {
		channel.addOperator(userSocket);
		std::string plusMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "mode/" + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "[+o " + memberClient->getUser().getNickname() + "] by " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "\r\n";
		std::string plusMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " +o " + tokens[3] + "\r\n";

		const std::vector<int>& members = channel.getMembers();
		for (size_t i = 0; i < members.size(); ++i) {
			int memberSocket = members[i];
			Client& memberClient = _clients[memberSocket];

			if (memberClient.isIrssi)
				fullMessage = plusMsgIrssi;
			else
				fullMessage = plusMsgNetcat;
			::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
		}
	}
	else {
		std::string netcatMessage = std::string(RED) + "Error: User " + tokens[3] + " not found in the channel: [" + channelName + "]\n" + std::string(RESET);
		std::string irssiMessage = ":localhost 401 " + client.getUser().getNickname() + " " + tokens[3] + " :No such nick/channel\r\n";
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}
}

void Server::modeMinusdOCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
	int userSocket = -1;
	Client* memberClient = NULL;
	std::string fullMessage;
	for (std::vector<int>::const_iterator it = channel.getMembers().begin(); it != channel.getMembers().end(); ++it) {
		Client& tempClient = getClientBySocket(*it);
		if (tempClient.getUser().getNickname() == tokens[3]) {
			userSocket = tempClient.getClientSocket();
			memberClient = &tempClient;
			break;
		}
	}
	if (userSocket != -1 && memberClient != NULL) {
		channel.removeOperator(userSocket);
		std::string minusMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "mode/" + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "[-o " + memberClient->getUser().getNickname() + "] by " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "\r\n";
		std::string minusMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " -o " + tokens[3] + "\r\n";

		const std::vector<int>& members = channel.getMembers();
		for (size_t i = 0; i < members.size(); ++i) {
			int memberSocket = members[i];
			Client& memberClient = _clients[memberSocket];

			if (memberClient.isIrssi)
				fullMessage = minusMsgIrssi;
			else
				fullMessage = minusMsgNetcat;
			::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
		}
	}
	else {
		std::string netcatMessage = std::string(RED) + "Error: User " + tokens[3] + " not found in the channel: [" + channelName + "]\n" + std::string(RESET);
		std::string irssiMessage = ":localhost 401 " + client.getUser().getNickname() + " " + tokens[3] + " :No such nick/channel\r\n";
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}
}

void	Channel::addOperator(int clientToAdd) {
	if (!isOperator(clientToAdd))
		_membersOperators.push_back(clientToAdd);
}

void	Channel::removeOperator(int clientToErase) {
	std::vector<int>::iterator it;
	for (it = _membersOperators.begin(); it != _membersOperators.end(); ++it) {
		if (*it == clientToErase) {
			_membersOperators.erase(it);
			break; 
		}
	}
}

bool Channel::isOperator(int clientSocket) const {
	std::vector<int>::const_iterator it;
	for (it = _membersOperators.begin(); it != _membersOperators.end(); ++it) {
		if (*it == clientSocket)
			return (true);
	}
	return (false);
}
