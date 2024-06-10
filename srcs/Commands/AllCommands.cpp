#include "../../includes/Server.hpp"

enum CommandType {
	HELP,
	CHANNEL,
	MSG,
	QUIT,
	LIST,
	NICK,
	WHOIS,
	UNKNOWN
};

CommandType getCommandType(const std::string& command) {
	if (command == "/help") return HELP;
	if (command == "/channel") return CHANNEL;
	if (command == "/msg") return MSG;
	if (command == "/quit") return QUIT;
	if (command == "/list") return LIST;
	if (command == "/nick" || "NICK") return NICK;
	if (command == "/whois") return WHOIS;
	return UNKNOWN;
}

void Server::parseClientMsg(const std::string& message, Client& client) {

		(void)message;
		(void)client;
	std::cout << BOLD << "\n" << client.getUser().getNickname() << " msg: " << RESET << message << std::endl;

	std::vector<std::string> tokens = split(message);
	if (tokens.empty()) {
		client.sendClientMsg(client.getClientSocket(), UNKNOWN_CMD);
		return;
	}

	for (std::vector<std::string>::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
		// std::cout << *it << std::endl;
		std::cout << CYAN << *it << RESET << std::endl;

	}

	for (size_t i = 0; i < tokens.size(); ++i) {
		std::cout << ORANGE << "i = " << i << RESET << std::endl;
		// std::cout << ORANGE << "tokens = " << tokens << RESET << std::endl;
		if (i >= 4) {
			std::cout << ORANGE << "I HAVE TOO MANY TOKENS OMG !!!!!!!!!!!" << RESET << std::endl;
			return;
		}
    }

	CommandType command = getCommandType(tokens[0]);
	switch (command) {
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
		case UNKNOWN:
		default:
			client.sendClientMsg(client.getClientSocket(), UNKNOWN_CMD);
			break;
	}
}