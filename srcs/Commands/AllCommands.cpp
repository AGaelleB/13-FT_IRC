#include "../../includes/Server.hpp"

CommandType Server::getCommandType(const std::string& command) {
	if (command == "/help") return HELP;
	if (command == "/nick" || command == "NICK") return NICK;
	if (command == "/list" || command == "LIST") return LIST;
	if (command == "/msg" || command == "PRIVMSG" || command == "/PRIVMSG") return PRIVMSG;
	if (command == "/channel" || command == "/join" || command == "JOIN") return CHANNEL; 
	if (command == "/quit") return QUIT;
	if (command == "/part" || command == "/leave" || command == "PART") return PART;
	if (command == "/topic") return TOPIC;
	if (command == "/kick") return KICK;
	if (command == "/invite") return INVITE;
	if (command == "PING") return PING;
	if (command == "MODE") return MODE;
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

	// Traitement des commandes après l'initialisation
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
			quitMsgCmdClient(client, tokens);
			break;
		case PART:
			std::cout << "PART command received" << std::endl;
			leaveChannel(client, tokens);
			break;
		case TOPIC:
			std::cout << "TOPIC command received" << std::endl;
			break;
		case KICK:
			std::cout << "KICK command received" << std::endl;
			break;
		case INVITE:
			std::cout << "INVITE command received" << std::endl;
			break;
		case PING:
			pingCmdClient(tokens, client);
			break;
		case UNKNOWN:
		default:
			// si la commande ne commence pas par un slash 
			// et que lutilisateur est present dans un channel le message senvoie dans le dernier chanel rejoint
			checkUnknownCmd(client, tokens);
			// client.sendClientMsg(client.getClientSocket(), UNKNOWN_CMD);
			break;
	}
}

void Server::checkUnknownCmd(Client& client, std::vector<std::string> tokens) {
	//verif taille tokens
    if (tokens.size() < 1) {
        client.sendClientMsg(client.getClientSocket(), ERROR_CMD_PRIVMSG);
        return;
    }

	if (tokens[0] == "/") {
		client.sendClientMsg(client.getClientSocket(), UNKNOWN_CMD);
		return;
	}
	// if (ChannelMsg(client)) {
		ChannelMsg(client);
	// }
		//ajouter /PRIVMSG un # et chanel name
	
	// else {
	// 	client.sendClientMsg(client.getClientSocket(), UNKNOWN_CMD);
	// 	return;
	// }


}


// /connect localhost 6667 1



/* TO DOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO

	gerer la 1er cmd gaelle msg: MODE gaelle +i pour pqs auelle ne soit  Unknown command ❌
		=> sera gerer lors du code de la cmd MODE 

*/