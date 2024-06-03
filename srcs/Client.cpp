
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
	std::cout << BOLD << client.getUser().getUsername()  <<  " msg: " << RESET << message;

	if (message.substr(0, 6) == "/login")
		std::cout << "Login command received" << std::endl;
	// else if (message.substr(0, 8) == "/channel")
	// 	std::cout << "Channel command received" << std::endl;
	else
		std::cout << "Unknown command\n" << std::endl;
	(void)client;
}

void Client::sendClientMsg(int client_socket, const char* message) {
	if (send(client_socket, message, strlen(message), 0) == -1) {
		std::cerr << "Error: failed to send message" << std::endl;
	}
}

void Client::welcomeClient(int client_socket) {
	const char* welcomeMsg = "\n ~~~ Welcome on our IRC Server! ~~~ \n\n";
	sendClientMsg(client_socket, welcomeMsg);
}