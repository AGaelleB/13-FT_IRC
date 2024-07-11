#include "../../includes/Server.hpp"

CommandType Server::getCommandType(const std::string& command) {
	if (command == "/help") return HELP;
	if (command == "/nick" || command == "NICK") return NICK;
	if (command == "/list" || command == "LIST") return LIST;
	if (command == "/msg" || command == "/PRIVMSG" || command == "PRIVMSG" || command == "/privmsg") return PRIVMSG;
	if (command == "/channel" || command == "/join" || command == "JOIN") return CHANNEL; 
	if (command == "/quit" || command == "/QUIT" || command == "QUIT") return QUIT;
	if (command == "/part" || command == "/leave" || command == "PART") return PART;
	if (command == "/topic" || command == "/TOPIC" || command == "TOPIC") return TOPIC;
	if (command == "/kick") return KICK;
	if (command == "/invite") return INVITE;
	if (command == "PING") return PING;
	if (command == "/mode" || command == "/MODE" ||command == "MODE") return MODE;
	return UNKNOWN;
}

void Server::parseClientMsg(const std::string& message, Client& client) {
	std::vector<std::string> tokens = split(message);
	if (tokens.empty()) {
		client.sendClientMsg(client.getClientSocket(), UNKNOWN_CMD);
		return;
	}

	std::string command = tokens[0];

	// Reconstituer le message avec ':' collé au message proprement dit
	std::string reconstructedMessage = command;
	bool colonAdded = false;
	for (size_t i = 1; i < tokens.size(); ++i) {
		if (i == 1) {
			reconstructedMessage += " " + tokens[i] + ":";
			colonAdded = true;
		} else {
			reconstructedMessage += tokens[i] + " ";
		}
	}

	// S'assurer que le ':' est ajouté correctement
	if (!colonAdded && tokens.size() > 1) {
		reconstructedMessage += ":";
	}

	// Supprimer l'espace supplémentaire à la fin du message reconstitué
	if (!reconstructedMessage.empty() && reconstructedMessage[reconstructedMessage.size() - 1] == ' ') {
		reconstructedMessage.erase(reconstructedMessage.size() - 1);
	}

	// Print le message reconstitué avec ':' collé au message proprement dit
	std::cout << BOLD << "\n<" << client.getUser().getNickname() << ">: " << RESET << reconstructedMessage << std::endl;

	CommandType commandType = getCommandType(command);
	switch (commandType) {
		case HELP:
			std::cout << "/help command received" << std::endl;
			helpCmdClient(client);
			break;
		case NICK:
			std::cout << "Nickname change command received" << std::endl;
			nickCmdClient(tokens, client);
			break;
		case LIST:
			std::cout << "List command received" << std::endl;
			listCmdClient(tokens, client);
			break;
		case PRIVMSG:
			std::cout << "Private message command received" << std::endl;
			privMsgCmdClient(client, tokens, reconstructedMessage);
			break;
		case CHANNEL:
			std::cout << "Channel command received" << std::endl;
			joinChannel(client, tokens);
			break;
		case QUIT:
			std::cout << "Quit command received" << std::endl;
			quitCmdClient(client, tokens);
			break;
		case PART:
			std::cout << "PART command received" << std::endl;
			leaveChannel(client, tokens);
			break;
		case TOPIC:
			std::cout << "TOPIC command received" << std::endl;
			topicCmdClient(client, tokens);
			break;
		case KICK:
			std::cout << "KICK command received" << std::endl;
			break;
		case INVITE:
			std::cout << "INVITE command received" << std::endl;
			break;
		case MODE:
			std::cout << "MODE command received" << std::endl;
			modeCmdClient(client, tokens);
			break;
		case PING:
			pingCmdClient(tokens, client);
			break;
		case UNKNOWN:
		default:
			// si la commande ne commence pas par un slash 
			// et que lutilisateur est present dans un channel le message senvoie dans le dernier chanel rejoint
			checkUnknownCmd(client, tokens);
			break;
	}
}


// a decouper pour plus de clarete 
void Server::checkUnknownCmd(Client& client, const std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_PRIVMSG);
		return;
	}

	if (tokens[0][0] == '/') {
		client.sendClientMsg(client.getClientSocket(), UNKNOWN_CMD);
		return;
	}

	std::string message = tokens[0];
	for (size_t i = 1; i < tokens.size(); ++i) {
		message += " " + tokens[i];
	}

	std::vector<std::string>::reverse_iterator rit;
	for (rit = _channelOrder.rbegin(); rit != _channelOrder.rend(); ++rit) {
		const std::string& channelName = *rit;
		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
		if (it != _channels.end() && it->second.isMember(client.getClientSocket())) {
			Channel& channel = it->second;

			// Utiliser une référence non-constante pour les membres
			const std::vector<int>& members = channel.getMembers();
			std::vector<int>::const_iterator memberIt;
			for (memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
				int memberSocket = *memberIt;
				Client& memberClient = _clients[memberSocket];

				std::string fullMessage;
				if (memberClient.isIrssi)
					fullMessage = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@localhost PRIVMSG " + channelName + " :" + message + "\r\n";
				else
					fullMessage = "[" + channelName + "] < " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "> " + message + "\r\n";

				send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
			}
			return;
		}
	}
	client.sendClientMsg(client.getClientSocket(), ERROR_NOT_IN_CHANNEL);
}

// /connect localhost 6667 1
