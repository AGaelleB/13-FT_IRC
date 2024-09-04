
#include "../../includes/Client.hpp"

/************************************* CONST ET DEST *************************************/

Client::Client() : _client_socket(-1) {
	memset(&_client_addr, 0, sizeof(_client_addr));
	isIrssi = false; 
	startServer = false;
}

Client::~Client() {
}

/*************************************** GETTERS ***************************************/

int Client::getClientSocket() const {
	return (_client_socket);
}

const struct sockaddr_in& Client::getClientAddr() const {
	return (_client_addr);
}

const User& Client::getUser() const {
	return (_user);
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
			std::cerr << RED << "Error: failed to send message" << RESET << std::endl;
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

        // Vérifier l'état du client en utilisant poll_count
        if (poll_count <= 0) {
            std::cerr << "Poll error or timeout during username entry." << std::endl;
            return "";
        }

        bytes_received = recv(this->getClientSocket(), buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cerr << RED << "Client disconnected during username entry ❌ [socket: " << this->getClientSocket() << RESET << std::endl;
            return "";
        }

        buffer[bytes_received] = '\0';
        std::string username = trim(buffer);

        if (username.length() < MIN_NAME_SIZE || username.length() > MAX_NAME_SIZE) {
            this->sendClientMsg(this->getClientSocket(), ERROR_USERNAME_LENGHT);
            continue;
        }

        if (this->checkName(username)) {
            return username;
        }

        this->sendClientMsg(this->getClientSocket(), ERROR_USERNAME);
    }
}

std::string Client::setNickName(Server& server) {
    char buffer[1024];
    ssize_t bytes_received;

    while (true) {
        this->sendClientMsg(this->getClientSocket(), MSG_NICKNAME);

        // Vérifier l'état du client en utilisant poll_count
        if (poll_count <= 0) {
            std::cerr << "Poll error or timeout during nickname entry." << std::endl;
            server.handleClientDisconnection(this->getClientSocket());
            return "";
        }

        bytes_received = recv(this->getClientSocket(), buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cerr << RED << "Client disconnected during nickname entry ❌ [socket: " << this->getClientSocket() << RESET << std::endl;
            server.handleClientDisconnection(this->getClientSocket());
            return "";
        }

        buffer[bytes_received] = '\0';
        std::string nickname = trim(buffer);

        if (nickname.length() < MIN_NAME_SIZE || nickname.length() > MAX_NAME_SIZE) {
            this->sendClientMsg(this->getClientSocket(), ERROR_NICKNAME_LENGHT);
            continue;
        }

        if (this->checkName(nickname)) {
            if (server.isNicknameAvailable(nickname)) {
                return nickname;
            } else {
                this->sendClientMsg(this->getClientSocket(), ERROR_NICKNAME_NOT_AVAILABLE);
            }
        } else {
            this->sendClientMsg(this->getClientSocket(), ERROR_NICKNAME);
        }
    }
}
