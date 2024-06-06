
#include "../includes/Client.hpp"

/************************************* CONST ET DEST *************************************/

Client::Client() : _client_socket(-1) {
	memset(&_client_addr, 0, sizeof(_client_addr));
}

Client::~Client() {
	// if (_client_socket != -1)
	// 	close(_client_socket);
}

/*************************************** GETTERS ***************************************/

int Client::getClientSocket() const {
	return (_client_socket);
}

struct sockaddr_in& Client::getClientAddr() {
	return (_client_addr);
}

User& Client::getUser() {
	return (_user);
}

/*************************************** SETTERS ***************************************/

void Client::setClientSocket(int socket) {
	_client_socket = socket;
}

void Client::setClientAddr(const struct sockaddr_in& addr) {
	_client_addr = addr;
}

void Client::setUser(const User& user) {
	_user = user;
}

/************************************** FUNCTIONS **************************************/

void Client::sendClientMsg(int client_socket, const char* message) {
	if (client_socket != -1) {
		if (send(client_socket, message, strlen(message), 0) == -1)
			std::cerr << "Error: failed to send message" << std::endl;
	}
}

void Client::helpCmdClient() {
	std::string helpMessage = 
		"\nAvailable Commands for clients:\n\n"
		"Command                   | Action\n"
		"--------------------------|----------------------------------------\n"
		"/help                     | Show the list of available cmds\n"
		"/channel [channel_name]   | Join/create a channel\n"
		"/msg [username] [message] | Send a private msg to a specific user\n"
		"/quit                     | Disconnect from the server\n"
		"/list user                | List all available users\n"
		"/list channel             | List all available channels\n"
		"/nick [new_nickname]      | Change your nickname\n"
		"/whois [username]         | Show info about a specific user\n\n";
	sendClientMsg(this->getClientSocket(), helpMessage.c_str());
}

void Server::removeNickname(const std::string& nickname) {
    std::cout << MAGENTA << "remove 1 old nickname : " << nickname << RESET << std::endl;
    _nicknames.erase(nickname);
    std::cout << MAGENTA << "remove 2 old nickname : " << nickname << RESET << std::endl;
    
    if (_nicknames.find(nickname) == _nicknames.end()) {
        std::cout << MAGENTA << "Nickname " << nickname << " successfully removed." << RESET << std::endl;
    } else {
        std::cout << MAGENTA << "Failed to remove nickname: " << nickname << RESET << std::endl;
    }
}

void	Client::nickCmdClient(std::vector<std::string> tokens, Server server) {
	std::string new_nickname = tokens[1];
	std::cout << new_nickname << std::endl;
	
	new_nickname.erase(std::remove(new_nickname.begin(), new_nickname.end(), '\n'), new_nickname.end());
	if (new_nickname.empty()) {
		this->sendClientMsg(this->getClientSocket(), ERROR_NEW_NICKNAME);
		return;
	}
	if (!this->checkName(new_nickname)) {
		this->sendClientMsg(this->getClientSocket(), ERROR_NICKNAME);
		return;
	}
	if (server.isNicknameAvailable(new_nickname)) {
		
		std::string old_nickname = getUser().getNickname();
        server.removeNickname(old_nickname);
		std::cout << MAGENTA << "cmd old nickname : " << old_nickname << RESET << std::endl;
		// ici faire une suppression de l'ancien nickname pour le rendre accessible une fois que le client a changer
		getUser().setNickname(new_nickname);
	}
	else
		this->sendClientMsg(this->getClientSocket(), ERROR_NICKNAME_NOT_AVAILABLE);
}

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

std::vector<std::string> split(const std::string &str) {
	std::vector<std::string> tokens;
	size_t pos = str.find_first_of(" \n");

	if (pos != std::string::npos) {
		tokens.push_back(str.substr(0, pos));
		if (pos + 1 < str.length())
			tokens.push_back(str.substr(pos + 1));
	}
	else
		tokens.push_back(str);
	return (tokens);
}

CommandType getCommandType(const std::string& command) {
	if (command == "/help") return HELP;
	if (command == "/channel") return CHANNEL;
	if (command == "/msg") return MSG;
	if (command == "/quit") return QUIT;
	if (command == "/list") return LIST;
	if (command == "/nick") return NICK;
	if (command == "/whois") return WHOIS;
	return UNKNOWN;
}

void Client::parseClientMsg(const std::string& message, Server server) {
	std::cout << BOLD << "\n" << getUser().getNickname() << " msg: " << RESET << message << std::endl;

	std::vector<std::string> tokens = split(message);
	if (tokens.empty()) {
		sendClientMsg(this->getClientSocket(), UNKNOWN_CMD);
		return;
	}
	// for (std::vector<std::string>::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
	// std::cout << *it << std::endl;
	// }

	CommandType command = getCommandType(tokens[0]);
	switch (command) {
		case HELP:
			std::cout << "/help command received" << std::endl;
			helpCmdClient();
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
			nickCmdClient(tokens, server);
			break;
		case WHOIS:
			std::cout << "Whois command received" << std::endl;
			break;
		case UNKNOWN:
		default:
			sendClientMsg(this->getClientSocket(), UNKNOWN_CMD);
			break;
	}
}

bool Client::checkName(const std::string& username) {
	std::string checkName = trim(username);

	if (checkName.length() > 15)
		return (false);
	for (size_t i = 0; i < checkName.length(); ++i) {
		if (!std::isalpha(checkName[i]) && checkName[i] != '_' && checkName[i] != '-')
			return (false); 
	}
	return (true); 
}

std::string Client::setUserName() {
	char buffer[1024];
	ssize_t bytes_received;

	while (true) {
		this->sendClientMsg(this->getClientSocket(), MSG_USERNAME);
		while (true) {
			bytes_received = recv(this->getClientSocket(), buffer, sizeof(buffer) - 1, 0);
			if (bytes_received == -1 && errno == EWOULDBLOCK) {
				usleep(42);
				continue;
			} else if (bytes_received > 0) {
				break;
			} else {
				if (bytes_received == 0)
					std::cout << RED << "\nClient disconnected during username entry ❌ ---> client_socket: " << this->getClientSocket() << RESET << std::endl;
				else
					std::cerr << "Error: reception failed during username entry, client_socket: " << this->getClientSocket() << std::endl;
				close(this->getClientSocket());
				return "";
			}
		}
		buffer[bytes_received] = '\0';
		if (this->checkName(std::string(buffer)) == true)
			return trim(std::string(buffer));
		this->sendClientMsg(this->getClientSocket(), ERROR_USERNAME);
	}
}

std::string Client::setNickName(Server& server) {
	char buffer[1024];
	ssize_t bytes_received;

	while (true) {
		this->sendClientMsg(this->getClientSocket(), MSG_NICKNAME);

		while (true) {
			bytes_received = recv(this->getClientSocket(), buffer, sizeof(buffer) - 1, 0);
			if (bytes_received == -1 && errno == EWOULDBLOCK) {
				usleep(42);
				continue;
			}
			else if (bytes_received > 0) {
				break;
			}
			else {
				if (bytes_received == 0)
					std::cout << RED << "\nClient disconnected during nickname entry ❌ ---> client_socket: " << this->getClientSocket() << RESET << std::endl;
				else
					std::cerr << "Error: reception failed during nickname entry, client_socket: " << this->getClientSocket() << std::endl;
				close(this->getClientSocket());
				return "";
			}
		}
		buffer[bytes_received] = '\0';
		std::string nickname = trim(std::string(buffer));
		if (!this->checkName(nickname)) {
			this->sendClientMsg(this->getClientSocket(), ERROR_NICKNAME);
			continue;
		}
		if (server.isNicknameAvailable(nickname))
			return nickname;
		else
			this->sendClientMsg(this->getClientSocket(), ERROR_NICKNAME_NOT_AVAILABLE);
	}
}
