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

	// Utiliser _channelOrder pour l'ordre d'affichage des canaux
	for (std::vector<std::string>::iterator it = _channelOrder.begin(); it != _channelOrder.end(); ++it) {
		std::string message = "\nChannel: " + *it;
		client.sendClientMsg(client.getClientSocket(), message.c_str());
		channelListMembers(client.getClientSocket(), *it, client);
	}

	client.sendClientMsg(client.getClientSocket(), MSG_END_LIST);
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
	return (membersList);
}

// /connect localhost 6667 1

