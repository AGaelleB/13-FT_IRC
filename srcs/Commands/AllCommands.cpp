#include "../../includes/Server.hpp"

enum CommandType {
	HELP,
	NICK,
	LIST,
	PRIVMSG,
	CHANNEL,
	QUIT,
	JOIN,
	PART,
	TOPIC,
	KICK,
	INVITE,
	PING,
	USER,
	MODE,
	UNKNOWN
};

CommandType getCommandType(const std::string& command) {
	if (command == "/help") return HELP;
	if (command == "/nick" || command == "NICK") return NICK;
	if (command == "/list" || command == "LIST") return LIST;
	if (command == "/msg" || command == "PRIVMSG") return PRIVMSG;
	if (command == "/channel") return CHANNEL;
	if (command == "/quit") return QUIT;
	if (command == "/join") return JOIN;
	if (command == "/part") return PART;
	if (command == "/topic") return TOPIC;
	if (command == "/kick") return KICK;
	if (command == "/invite") return INVITE;
	if (command == "PING") return PING;
	if (command == "USER") return USER;
	if (command == "MODE") return MODE;
	return UNKNOWN;
}

void Server::parseClientMsg(const std::string& message, Client& client) {
	std::cout << BOLD << "\n" << client.getUser().getNickname() << " msg: " << RESET << message << std::endl;

	std::vector<std::string> tokens = split(message);
	if (tokens.empty()) {
		client.sendClientMsg(client.getClientSocket(), UNKNOWN_CMD);
		return;
	}

	std::string command = tokens[0];
	// std::cout << "Command received: " << command << std::endl;

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
			if (tokens.size() != 2) {
				client.sendClientMsg(client.getClientSocket(), ERROR_CMD_LIST);
				return;
			}
			listCmdClient(tokens[1], client);
			break;
		case PRIVMSG:
			std::cout << "Private message command received" << std::endl;
			if (tokens.size() < 3) {
				client.sendClientMsg(client.getClientSocket(), ERROR_CMD_PRIVMSG);
				return;
			}
			// privMsgCmdClient(client, tokens[1], tokens[2]); ////////////////////////
			privMsgCmdClient(client, tokens[1], tokens[2]); // Join the remaining tokens as the message
			break;
		case CHANNEL:
			std::cout << "Channel command received" << std::endl;
			break;
		case QUIT:
			std::cout << "Quit command received" << std::endl;
			break;
		case JOIN:
			std::cout << "JOIN command received" << std::endl;
			break;
		case PART:
			std::cout << "PART command received" << std::endl;
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
		// case USER:
		// 	std::cout << "USER command received" << std::endl;
		// 	if (tokens.size() < 2) {
		// 		client.sendClientMsg(client.getClientSocket(), ERROR_NEW_USERNAME);
		// 		return;
		// 	}
		// 	client.getUser().setUsername(tokens[1]);
		// 	std::cout << "Username set to " << tokens[1] << std::endl;
		// 	break;
		// case MODE:
		// 	std::cout << "MODE command received" << std::endl;
		// 	if (tokens.size() < 3) {
		// 		client.sendClientMsg(client.getClientSocket(), ERROR_MODE);
		// 		return;
		// 	}
		// 	handleMode(client, tokens[1], tokens[2]);
		// 	break;
		case PING:
			if (tokens.size() > 1) {
				// std::cout << ORANGE << "JE TE REPONDS A TON PING" << RESET << std::endl;
				std::string response = RPL_PONG(client.getUser().getNickname(), tokens[1]);
				client.sendClientMsg(client.getClientSocket(), response.c_str());
			}
			break;
		case UNKNOWN:
		default:
			client.sendClientMsg(client.getClientSocket(), UNKNOWN_CMD);
			break;
	}
}


// void Server::handleMode(Client& client, const std::string& channel, const std::string& mode) {
// 	// Gérer les changements de mode de canal
// 	std::cout << "Changing mode for channel " << channel << " to " << mode << std::endl;
// 	// Logique pour changer le mode du canal
// }



// /connect localhost 6667 1



/* TO DOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO

	gerer la 1er cmd gaelle msg: MODE gaelle +i pour pqs auelle ne soit  Unknown command ❌
		=> sera gerer lors du code de la cmd MODE 

	faire la cmd msg

 */