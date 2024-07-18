#include "../../includes/Server.hpp"

/************************************** LIST USERS **************************************/

void Server::listCmdClient(std::vector<std::string> tokens, Client& client) {
	if (tokens.size() != 2) {
		std::string errorNetcat = std::string(BOLD) + "Error: Invalid command format for LIST\n" + std::string(RESET);
		std::string errorIrssi = ":localhost 421 " + client.getUser().getNickname() + " LIST :Unknown command\r\n";
		sendErrorMessage(client, errorNetcat, errorIrssi);
		return;
	}

	std::string trim_token = trim(tokens[1]);
	if (tokens[1] == "channel")
		channelList(client);
	else if (trim_token == "user")
		UserList(client);
	else {
		std::string errorNetcat = std::string(BOLD) + "Error: Invalid parameter for LIST\n" + std::string(RESET);
		std::string errorIrssi = ":localhost 421 " + client.getUser().getNickname() + " LIST :Unknown command\r\n";
		sendErrorMessage(client, errorNetcat, errorIrssi);
	}
}

void Server::UserList(Client& client) {
	std::string nicknames_list;
	
	std::string msgSendUserNetcat = std::string(BOLD) + "Listing users:\n" + std::string(RESET);
	std::string msgSendUserIrssi = ":localhost 392 " + client.getUser().getNickname() + " :User list\r\n";
	sendErrorMessage(client, msgSendUserNetcat, msgSendUserIrssi);

	for (std::set<std::string>::iterator it = _nicknames.begin(); it != _nicknames.end(); ++it)
		nicknames_list += *it + "\n";

	std::string msgEndListNetcat = std::string(BOLD) + "End of user list\n" + std::string(RESET);
	std::string msgEndListIrssi = ":localhost 394 " + client.getUser().getNickname() + " :End of user list\r\n";
	sendErrorMessage(client, nicknames_list, nicknames_list); // Send the user list in both formats
	sendErrorMessage(client, msgEndListNetcat, msgEndListIrssi);
}

/************************************ LIST CHANNELS ***********************************/

void Server::channelListMembers(int clientSocket, const std::string& channelName, Client& client) {
	
	(void)clientSocket;
	
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it != _channels.end()) {
		const std::vector<int>& members = it->second.getMembers();
		for (std::vector<int>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
			std::map<int, Client>::iterator clientIt = _clients.find(*memberIt);
			if (clientIt != _clients.end()) {
				std::string memberMsgNetcat = std::string(BOLD) + "	-> " + clientIt->second.getUser().getNickname() + "\n" + std::string(RESET);
				std::string memberMsgIrssi = ":localhost 353 " + client.getUser().getNickname() + " = " + channelName + " :@" + clientIt->second.getUser().getNickname() + "\r\n";
				sendErrorMessage(client, memberMsgNetcat, memberMsgIrssi);
			}
		}
	}
}

void Server::channelList(Client& client) {
	std::string msgSendChannelNetcat = std::string(BOLD) + "Listing channels:\n" + std::string(RESET);
	std::string msgSendChannelIrssi = ":localhost 321 " + client.getUser().getNickname() + " :Channel list\r\n";
	sendErrorMessage(client, msgSendChannelNetcat, msgSendChannelIrssi);

	for (std::vector<std::string>::iterator it = _channelOrder.begin(); it != _channelOrder.end(); ++it) {
		std::string channelMsgNetcat = std::string(BOLD) + "Channel: " + *it + "\n" + std::string(RESET);
		std::string channelMsgIrssi = ":localhost 322 " + client.getUser().getNickname() + " " + *it + " :\r\n";
		sendErrorMessage(client, channelMsgNetcat, channelMsgIrssi);
		channelListMembers(client.getClientSocket(), *it, client);
	}

	std::string msgEndListNetcat = std::string(BOLD) + "End of channel list\n" + std::string(RESET);
	std::string msgEndListIrssi = ":localhost 323 " + client.getUser().getNickname() + " :End of channel list\r\n";
	sendErrorMessage(client, msgEndListNetcat, msgEndListIrssi);
}

std::string Server::PrintChannelListMembers(const std::string& channelName, const std::map<std::string, Channel>& channels) {
	std::map<std::string, Channel>::const_iterator it = channels.find(channelName);
	if (it == channels.end()) {
		return "";
	}

	const std::vector<int>& members = it->second.getMembers();
	std::string membersList;

	for (std::vector<int>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
		std::map<int, Client>::const_iterator clientIt = _clients.find(*memberIt);
		if (clientIt != _clients.end()) {
			membersList += "<" + clientIt->second.getUser().getNickname() + "> ";
		}
	}

	// Supprimer le dernier espace
	if (!membersList.empty()) {
		membersList.erase(membersList.size() - 1);
	}
	membersList = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "-" + std::string(RESET) + " [" + std::string(CYAN_IRSSI) + "users list" + std::string(RESET) + "] " + std::string(CYAN_IRSSI) + membersList + std::string(RESET) + "\r\n";
	return membersList;
}



// /connect localhost 6667 1

