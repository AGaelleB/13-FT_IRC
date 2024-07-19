#include "../../../includes/Server.hpp"

/* 

i : Invitation seulement - le canal est accessible uniquement sur invitation.
t : Sujet restreint - seul les opérateurs peuvent changer le sujet.
k : Mot de passe - un mot de passe est requis pour rejoindre le canal.
o : Opérateur de canal - donne ou retire le privilège d'opérateur.
l : Limite d'utilisateurs - fixe un nombre maximum d'utilisateurs dans le canal.

*/

ModeType Server::getModeType(const std::string& modeType) {
	if (modeType == "+i" || modeType == "-i") return MODE_I;
	if (modeType == "+t" || modeType == "-t") return MODE_T;
	if (modeType == "+k" || modeType == "-k") return MODE_K;
	if (modeType == "+o" || modeType == "-o") return MODE_O;
	if (modeType == "+l" || modeType == "-l") return MODE_L;
	if (modeType == "+b" || modeType == "-b") return MODE_B;
	return MODE_UNKNOWN;
}

bool Server::validateTokensMode(Client& client, const std::vector<std::string>& tokens) {
	if (tokens.size() < 3) {
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
	
	if (!validateChannelMembership(client, channelName))
		return;

	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end()) {
		std::string irssiMessage = ":localhost 403 " + client.getUser().getNickname() + " " + channelName + " :No such channel\r\n";
		std::string netcatMessage = std::string(RED) + "Error: Channel " + channelName + " not found.\n" + RESET;
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}
	
	Channel& channel = it->second;
	std::string mode = tokens[2];

	ModeType modetype = getModeType(mode);
	switch (modetype) {
		case MODE_I:
			std::cout << "mode I received" << std::endl;
			modeICmd(client, tokens, channel, channelName);
			break;
		case MODE_T:
			std::cout << "mode T received" << std::endl;
			modeTCmd(client, tokens, channel, channelName);
			break;
		case MODE_K:
			std::cout << "mode K received" << std::endl;
			modeKCmd(client, tokens, channel, channelName);
			break;
		case MODE_O:
			std::cout << "mode O received" << std::endl;
			modeOCmd(client, tokens, channel, channelName);
			break;
		case MODE_L:
			std::cout << "mode L received" << std::endl;
			modeLCmd(client, tokens, channel, channelName);
			break;
		case MODE_B:
			std::cout << "mode B received" << std::endl;
			modeBCmd(client, tokens, channel, channelName);
			break;
		case MODE_UNKNOWN:
		default:
			std::string irssiMessage = ":localhost 472 " + client.getUser().getNickname() + " " + mode + " :is unknown mode char to me\r\n";
			std::string netcatMessage = std::string(RED) + "Error: Unknown mode command.\n" + RESET;
			sendErrorMessage(client, netcatMessage, irssiMessage);
			break;
	}
}

// /connect localhost 6667 1



/* 

	Définir un mot de passe pour le canal (+k).
	Limiter le nombre d'utilisateurs pouvant rejoindre le canal (+l).
	Kicker un utilisateur (KICK) : Les opérateurs peuvent expulser un utilisateur du canal.
	Changer le sujet du canal (TOPIC) : Seuls les opérateurs peuvent changer le sujet du canal si le mode +t (topic protection) est activé.

 */