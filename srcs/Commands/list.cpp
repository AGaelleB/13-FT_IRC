#include "../../includes/Server.hpp"



/************************************** LIST USERS **************************************/

void Server::listCmdClient(std::vector<std::string> tokens, Client& client) {

	if (tokens.size() != 2) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_LIST);
		return;
	}

	std::string trim_token = trim(tokens[1]);
	if (tokens[1] == "channel")
		channelList(client);
	else if (trim_token == "user")
		UserList(client);
	else
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_LIST);
}

void Server::UserList(Client& client) {
	std::string nicknames_list;

	client.sendClientMsg(client.getClientSocket(), MSG_SEND_USER);
	for (std::set<std::string>::iterator it = _nicknames.begin(); it != _nicknames.end(); ++it)
		nicknames_list += *it + "\n";

	// Envoyer la liste des surnoms au client
	client.sendClientMsg(client.getClientSocket(), nicknames_list.c_str());
	client.sendClientMsg(client.getClientSocket(), MSG_END_LIST);

}


/************************************* LIST CHANNELS ************************************/

std::string intToString(int number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void Server::channelListMembers(int clientSocket, const std::string& channelName, Client& client) {
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it != _channels.end()) {
		const std::vector<int>& members = it->second.getMembers();
		for (std::vector<int>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
			std::map<int, Client>::iterator clientIt = _clients.find(*memberIt);
			if (clientIt != _clients.end()) {
				std::string message = "\n	-> " + clientIt->second.getUser().getNickname();
				client.sendClientMsg(clientSocket, message.c_str());
			}
		}
	}
	client.sendClientMsg(clientSocket, "\n");
}

void Server::channelList(Client& client) {
	client.sendClientMsg(client.getClientSocket(), MSG_SEND_CHANNEL);

	std::map<std::string, Channel>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); ++it) {
		std::string message = "\nChannel: #" + it->first;
		client.sendClientMsg(client.getClientSocket(), message.c_str());
		channelListMembers(client.getClientSocket(), it->first, client);
	}

	client.sendClientMsg(client.getClientSocket(), MSG_END_LIST);
}