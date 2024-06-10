
#include "../../includes/Client.hpp"

/************************************* CONST ET DEST *************************************/

Client::Client() : _client_socket(100) {
	memset(&_client_addr, 0, sizeof(_client_addr));
}

Client::~Client() {
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
	char	buffer[1024];
	ssize_t	bytes_received;

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
	char	buffer[1024];
	ssize_t	bytes_received;

	while (true) {
		this->sendClientMsg(this->getClientSocket(), MSG_NICKNAME);
		while (true) {
			bytes_received = recv(this->getClientSocket(), buffer, sizeof(buffer) - 1, 0);
			if (bytes_received == -1 && errno == EWOULDBLOCK) {
				usleep(42);
				continue;
			}
			else if (bytes_received > 0)
				break;
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
