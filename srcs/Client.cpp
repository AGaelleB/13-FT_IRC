
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

void Client::handleClientMsg(const std::string& message, Client& client) {
	std::cout << BOLD << "\n" << client.getUser().getNickname()  <<  " msg: " << RESET << message;

	if (message.substr(0, 6) == "/login")
		std::cout << "Login command received" << std::endl;
	// else if (message.substr(0, 8) == "/channel")
	// 	std::cout << "Channel command received" << std::endl;
	else {
		const char* unknownCommand = "Unknown command ❌\n";
		sendClientMsg(this->getClientSocket(), unknownCommand);
	}
	(void)client;
}

void Client::sendClientMsg(int client_socket, const char* message) {
	if (send(client_socket, message, strlen(message), 0) == -1) {
		std::cerr << "Error: failed to send message" << std::endl;
	}
}

void Client::welcomeClient(int client_socket) {
	const char* welcomeMsg = BLUE "\n ~~~ Welcome on our IRC Server! ~~~ \n\n" RESET;
	sendClientMsg(client_socket, welcomeMsg);
}


bool Client::checkName(const std::string& username) {
	std::string checkName = trim(username);

	if (checkName.length() > 15)
		return (false);
	for (size_t i = 0; i < checkName.length(); ++i) {
		if (!std::isalpha(checkName[i]) && checkName[i] != '_' && checkName[i] != '-') {
			return (false); 
		}
	}
	return (true); 
}


std::string Client::setUserName() {
	char buffer[1024];
	ssize_t bytes_received;

	while (true)
	{
		const char* usernameMsg = BOLD "Enter your username: " RESET;
		this->sendClientMsg(this->getClientSocket(), usernameMsg);
		bytes_received = recv(this->getClientSocket(), buffer, sizeof(buffer) - 1, 0);
		if (bytes_received <= 0) {
			std::cerr << "Error: reception failed during username entry, client_socket: " << this->getClientSocket() << std::endl;
			close(this->getClientSocket());
			return (NULL);
		}
		buffer[bytes_received] = '\0';
		
		if (this->checkName(std::string(buffer)) == true) {
			return (trim(std::string(buffer)));
		}
		const char* errorUserName = RED "Error: Your username must be less than 15 characters and contain only alphanumeric characters\n" RESET;
		this->sendClientMsg(this->getClientSocket(), errorUserName);
	}
}

std::string Client::setNickName() {
	char buffer[1024];
	ssize_t bytes_received;

	while (true) {
		const char* nicknameMsg = BOLD "Enter your nickname: " RESET;
		this->sendClientMsg(this->getClientSocket(), nicknameMsg);
		bytes_received = recv(this->getClientSocket(), buffer, sizeof(buffer) - 1, 0);
		if (bytes_received <= 0) {
			std::cerr << "Error: reception failed during nickname entry, client_socket: " << this->getClientSocket() << std::endl;
			close(this->getClientSocket());
			return (NULL);
		}
		buffer[bytes_received] = '\0';
		if (this->checkName(std::string(buffer)) == true) {
			return (trim(std::string(buffer)));
		}
		const char* errorNickName = RED "Error: Your nickname must be less than 15 characters and contain only alphanumeric characters\n" RESET;
		this->sendClientMsg(this->getClientSocket(), errorNickName);
	}
}