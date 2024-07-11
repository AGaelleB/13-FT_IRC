#include "../../../includes/Server.hpp"


/* 

i : Invitation seulement - le canal est accessible uniquement sur invitation.
t : Sujet restreint - seul les opérateurs peuvent changer le sujet.
k : Mot de passe - un mot de passe est requis pour rejoindre le canal.
o : Opérateur de canal - donne ou retire le privilège d'opérateur.
l : Limite d'utilisateurs - fixe un nombre maximum d'utilisateurs dans le canal.

*/



// /connect localhost 6667 1

bool Server::validateTokensMode(Client& client, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE); 
		return (false);
	}
	return (true);
}


void Server::modeCmdClient(Client& client, std::vector<std::string> tokens) {
	if (!validateTokensMode(client, tokens))
		return;	
	
	std::string channelName = tokens[1];
	
	if (channelName[0] != '#')
		channelName = "#" + channelName;
	
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end()) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_NOT_FOUND);
		return;
	}
	
	Channel& channel = it->second;
	std::string mode = tokens[2];
	if (mode == "+k") {
		if (tokens.size() != 4) {
			client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE_K);
			return;
		}
		channel.setChannelKey(tokens[3]);
	}
	else if (mode == "-k") {
		channel.setChannelKey("");
	}
	else {
		client.sendClientMsg(client.getClientSocket(), ERROR_MODE_NOT_FOUND);
		return;
	}
}