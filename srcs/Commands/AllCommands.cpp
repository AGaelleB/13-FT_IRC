#include "../../includes/Server.hpp"

enum CommandType {
	HELP,
	CHANNEL,
	MSG,
	QUIT,
	LIST,
	NICK,
	WHOIS,
	PING,
	UNKNOWN
};

CommandType getCommandType(const std::string& command) {
	if (command == "/help") return HELP;
	if (command == "/channel") return CHANNEL;
	if (command == "/msg") return MSG;
	if (command == "/quit") return QUIT;
	if (command == "/list") return LIST;
	if (command == "/nick" || command == "NICK") return NICK;
	if (command == "/whois") return WHOIS;
	if (command == "PING") return PING;
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
		case CHANNEL:
			std::cout << "Channel command received" << std::endl;
			break;
		case MSG:
			std::cout << "Private message command received" << std::endl;
			break;
		case QUIT:
			std::cout << "Quit command received" << std::endl;
			break;
		case LIST:
			std::cout << "List command received" << std::endl;
			break;
		case NICK:
			std::cout << "Nickname change command received" << std::endl;
			nickCmdClient(tokens, client);
			break;
		case WHOIS:
			std::cout << "Whois command received" << std::endl;
			break;
		case PING: // Ajout du cas PING
			if (tokens.size() > 1) {
				std::cout << ORANGE << "JE TE REPONDS A TON PING" << RESET << std::endl;
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


// /connect localhost 6667 1