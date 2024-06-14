#include "../../includes/Server.hpp"

CommandType Server::getCommandType(const std::string& command) {
	if (command == "/help") return HELP;
	if (command == "/nick" || command == "NICK") return NICK;
	if (command == "/list" || command == "LIST") return LIST;
	if (command == "/msg" || command == "PRIVMSG") return PRIVMSG;
	if (command == "/channel" || command == "/join" || command == "JOIN") return CHANNEL; 
	if (command == "/quit") return QUIT;
	if (command == "/part" || command == "/leave") return PART;
	if (command == "/topic") return TOPIC;
	if (command == "/kick") return KICK;
	if (command == "/invite") return INVITE;
	if (command == "PING") return PING;
	if (command == "MODE") return MODE;
	return UNKNOWN;
}

void Server::parseClientMsg(const std::string& message, Client& client) {
	std::cout << BOLD << "\n<" << client.getUser().getNickname() << ">: " << RESET << message << std::endl;

	std::vector<std::string> tokens = split(message);
	if (tokens.empty()) {
		client.sendClientMsg(client.getClientSocket(), UNKNOWN_CMD);
		return;
	}

	std::string command = tokens[0];

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
			privMsgCmdClient(client, tokens, message);
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
			client.sendClientMsg(client.getClientSocket(), UNKNOWN_CMD);
			break;
	}
}



// /connect localhost 6667 1



/* TO DOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO

	gerer la 1er cmd gaelle msg: MODE gaelle +i pour pqs auelle ne soit  Unknown command ❌
		=> sera gerer lors du code de la cmd MODE 

*/