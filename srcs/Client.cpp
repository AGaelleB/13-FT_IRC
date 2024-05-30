
#include "../includes/Client.hpp"


/************************************* CONST ET DEST *************************************/

Client::Client() : _client_socket(-1) {
	memset(&_client_addr, 0, sizeof(_client_addr));
}

Client::~Client() {
	if (_client_socket != -1)
		close(_client_socket);
}


/*************************************** GETTERS ***************************************/

int Client::getClientSocket() const {
	return (_client_socket);
}

struct sockaddr_in& Client::getClientAddr() {
	return (_client_addr);
}


/*************************************** SETTERS ***************************************/

void Client::setClientSocket(int socket) {
	_client_socket = socket;
}

void Client::setClientAddr(const struct sockaddr_in& addr) {
	_client_addr = addr;
}


/************************************** FUNCTIONS **************************************/

void Client::handleClientMsg(const std::string& message, Client& client) {
	std::cout << "Client: " << message << std::endl; //suppr

	if (message.substr(0, 6) == "/login")
		std::cout << "Login command received" << std::endl;
	// else if (message.substr(0, 8) == "/channel")
	// 	std::cout << "Channel command received" << std::endl;
	else
		std::cout << "Unknown command" << std::endl;
	(void)client;
}

void Client::sendClientMsg(int client_socket, const char* message) {
	if (send(client_socket, message, strlen(message), 0) == -1) {
		std::cerr << "Error: failed to send message" << std::endl;
	}
}

void Client::welcomeClient(int client_socket) {
	const char*	welcomeMsg = BLUE "\n ~~~ Welcome on our IRC Server! ~~~ \n\n" RESET;
	const char*	loginMsg = BOLD "Enter your login: " RESET;

	sendClientMsg(client_socket, welcomeMsg);
	sendClientMsg(client_socket, loginMsg);
}
