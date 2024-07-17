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
	if (command == "/kick" || command == "/KICK" || command == "KICK") return KICK;
	if (command == "/invite" || command == "/INVITE" || command == "INVITE") return INVITE;
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
			joinCmdClient(client, tokens);
			break;
		case QUIT:
			std::cout << "Quit command received" << std::endl;
			quitCmdClient(client, tokens);
			break;
		case PART:
			std::cout << "PART command received" << std::endl;
			partCmdClient(client, tokens);
			break;
		case TOPIC:
			std::cout << "TOPIC command received" << std::endl;
			topicCmdClient(client, tokens);
			break;
		case KICK:
			std::cout << "KICK command received" << std::endl;
			kickCmdClient(client, tokens);
			break;
		case INVITE:
			std::cout << "INVITE command received" << std::endl;
			inviteCmdClient(client, tokens);
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
			handleUnknownCommand(client, tokens);
			break;
	}
}

void Server::handleUnknownCommand(Client& client, const std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		std::string netcatMessage = "Error: Must be: /msg OR /PRIVMSG <target> <message>\n";
		std::string irssiMessage = ":localhost NOTICE * :Must be: /msg OR /PRIVMSG <target> <message>\r\n";
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}

	if (tokens[0][0] == '/') {
		std::string netcatMessage = "Unknown command\n";
		// std::string irssiMessage = ":localhost NOTICE * :Unknown command\r\n";
		std::string irssiMessage = ERR_UNKNOWNCOMMAND(client.getUser().getNickname(), tokens[0]);
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}

	// Reconstituer le message à partir des tokens
	std::string message = tokens[0];
	for (size_t i = 1; i < tokens.size(); ++i)
		message += " " + tokens[i];

	// Parcourir la liste des canaux pour trouver ceux auxquels le client appartient
	for (std::vector<std::string>::reverse_iterator rit = _channelOrder.rbegin(); rit != _channelOrder.rend(); ++rit) {
		const std::string& channelName = *rit;
		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
		
		// Si le canal est trouvé et que le client est membre de ce canal
		if (it != _channels.end() && it->second.isMember(client.getClientSocket())) {
			Channel& channel = it->second;
			const std::vector<int>& members = channel.getMembers();
			
			// Envoyer le message à tous les membres du canal
			for (std::vector<int>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
				int memberSocket = *memberIt;
				Client& memberClient = _clients[memberSocket];
				std::string fullMessage;
				
				if (memberClient.isIrssi)
                    fullMessage = RPL_PRIVMSG(client.getUser().getNickname(), client.getUser().getUsername(), channelName, message);
					// fullMessage = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@localhost PRIVMSG " + channelName + " :" + message + "\r\n";
				else
					fullMessage = "[" + channelName + "] <" + BOLD + client.getUser().getNickname() + RESET + "> " + message + "\r\n";
				send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
			}
			return;
		}
	}

	// Si le client n'est membre d'aucun canal, envoyer un message d'erreur
	std::string netcatMessage = "ERROR: Failed to send message, the client must join a channel\n";
	// std::string irssiMessage = ":localhost NOTICE * :You are not in any channel\r\n";
	std::string irssiMessage = ERR_NOTONCHANNEL(client.getUser().getNickname(), "NoChannel");
	sendErrorMessage(client, netcatMessage, irssiMessage);
}


// /connect localhost 6667 1
