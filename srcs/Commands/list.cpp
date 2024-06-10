#include "../../includes/Server.hpp"

void Server::listCmdClient(std::string token, Client& client) {

	std::string trim_token = trim(token);
	if (token == "channel")
		channelList(client);
	else if (trim_token == "user")
		UserList(client);
	else
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_LIST);
}

void Server::channelList(Client& client) {
	(void)client;
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