#include "../../../includes/Server.hpp"

/* 

i : Invitation seulement - le canal est accessible uniquement sur invitation.
t : Sujet restreint - seul les opérateurs peuvent changer le sujet.
k : Mot de passe - un mot de passe est requis pour rejoindre le canal.
o : Opérateur de canal - donne ou retire le privilège d'opérateur.
l : Limite d'utilisateurs - fixe un nombre maximum d'utilisateurs dans le canal.

*/

// /connect localhost 6667 1

ModeType Server::getModeType(const std::string& modeType) {
	if (modeType == "+i" || modeType == "-i") return MODE_I;
	if (modeType == "+t" || modeType == "-t") return MODE_T;
	if (modeType == "+k" || modeType == "-k") return MODE_K;
	if (modeType == "+o" || modeType == "-o") return MODE_O;
	if (modeType == "+l" || modeType == "-l") return MODE_L;
	return MODE_UNKNOWN;
}


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


	ModeType modetype = getModeType(mode);
	switch (modetype) {
		case MODE_I:
			std::cout << "mode I received" << std::endl;
			break;
		case MODE_T:
			std::cout << "mode T received" << std::endl;
			break;
		case MODE_K:
			std::cout << "mode K received" << std::endl;
			modeKCmd(client, tokens, channel);
			break;
		case MODE_O:
			std::cout << "mode O received" << std::endl;
			modeOCmd(client, tokens, channel, channelName);
			break;
		case MODE_L:
			std::cout << "mode L received" << std::endl;
			break;
		case MODE_UNKNOWN:
		default:
			client.sendClientMsg(client.getClientSocket(), ERROR_MODE_NOT_FOUND);
			break;
	}
}
