#include "../includes/Server.hpp"

/************************************* CONST ET DEST *************************************/

Server::Server() : _server_socket(-1), _password("1234"), _port(6667) {
}

Server::Server(int port, const std::string &password) : _server_socket(-1), _password(password), _port(port) {
	// 1. Crée un socket: socket()
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1) {
		std::cerr << "Error: socket creation failed" << std::endl;
		exit(1);
	}

	// 2. Lie le socket à une adresse IP et un _port: bind(), htons()
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_addr.sin_port = htons(_port);

	if (bind(_server_socket, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) {
		std::cerr << "Error: bind failed" << std::endl;
		close(_server_socket);
		exit(1);
	}

	// 3. Met le socket en écoute pour les connexions entrantes: listen()
	if (listen(_server_socket, SOMAXCONN) == -1) {
		std::cerr << "Error: listen failed" << std::endl;
		close(_server_socket);
		exit(1);
	}

	struct pollfd server_fd;
	server_fd.fd = _server_socket;
	server_fd.events = POLLIN; // What is that !!?????
	_fds.push_back(server_fd); // why push back ?
}

Server::~Server() {
	if (_server_socket != -1)
		close(_server_socket);
}


/************************************** FUNCTIONS **************************************/

void Server::addUser(Client &client, const std::string &username, const std::string &nickname) {
	static int current_index = 1;
	User user(current_index++, username, nickname);
	client.setUser(user);
	_clients[client.getClientSocket()] = client;
}

std::string trim(const std::string& str) {
	size_t start = 0;
	while (start < str.size() && std::isspace(str[start])) {
		start++;
	}

	size_t end = str.size();
	while (end > start && std::isspace(str[end - 1])) {
		end--;
	}

	return (str.substr(start, end - start));
}

void Server::acceptClient() {
	Client client;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_socket = accept(_server_socket, (struct sockaddr*)&client_addr, &client_len);
	if (client_socket == -1) {
		std::cerr << "Error: connexion not accepted" << std::endl;
		return;
	}

	client.setClientSocket(client_socket);
	client.setClientAddr(client_addr);
	_clients[client_socket] = client;

	// Initialiser l'état du client
	ClientState clientState;
	clientState.client = client;
	clientState.state = WAITING_FOR_PASSWORD;
	_clientStates[client_socket] = clientState;

	struct pollfd client_fd;
	client_fd.fd = client_socket;
	client_fd.events = POLLIN;
	_fds.push_back(client_fd);

	client.sendClientMsg(client_socket, BOLD "Enter Server password: " RESET);

	std::cout << "\nNew connexion accepted ​✅" << std::endl;
}

void Server::registrationClient(Client client, int client_socket) {
	char buffer[1024];
	ssize_t bytes_received;

	while (true) {
		const char* pass_msg = BOLD "Enter Server password: " RESET;
		client.sendClientMsg(client_socket, pass_msg);

		bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
		if (bytes_received <= 0) {
			std::cerr << "Error: reception failed" << std::endl;
			close(client_socket);
			removeClient(client_socket);
			return;
		}
		buffer[bytes_received] = '\0';

		std::string pass(buffer);
		pass = trim(pass);

		if (pass != this->_password) {
			const char* invalid_pass = RED "Wrong password \n\n" RESET;
			client.sendClientMsg(client_socket, invalid_pass);
		}
		else
			break;
	}

	client.welcomeClient(client_socket);

	const char* putUsername = BOLD "Enter your username: " RESET;
	const char* putNickname = BOLD "Enter your nickname: " RESET;
	const char* isRegistered = GREEN "You are now registered! ​✅\n\n" RESET;

	std::cout << "\n. . . Waiting for client registration . . . " << std::endl;

	// Handle Username
	client.sendClientMsg(client_socket, putUsername);
	bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received <= 0) {
		std::cerr << "Error: reception failed" << std::endl;
		close(client_socket);
		removeClient(client_socket);
		return;
	}
	buffer[bytes_received] = '\0';
	std::string username = trim(std::string(buffer));

	// Handle Nickname
	client.sendClientMsg(client_socket, putNickname);
	bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received <= 0) {
		std::cerr << "Error: reception failed" << std::endl;
		close(client_socket);
		removeClient(client_socket);
		return;
	}
	buffer[bytes_received] = '\0';
	std::string nickname = trim(std::string(buffer));

	addUser(client, username, nickname);
	client.sendClientMsg(client_socket, isRegistered);
}


void Server::handleClientMessage(int client_socket) {
	char buffer[1024];
	ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received <= 0) {
		if (bytes_received == 0) {
			std::cout << "Client disconnected ❌" << std::endl;
		}
		else {
			std::cerr << "Error: reception failed" << std::endl;
		}
		removeClient(client_socket);
		return;
	}
	buffer[bytes_received] = '\0';
	std::string message = trim(std::string(buffer));

	ClientState &clientState = _clientStates[client_socket];

	std::cout << RED << "AVANT SWITCH CASE" << RESET <<  std::endl;
	switch (clientState.state) {
		case WAITING_FOR_PASSWORD:
			std::cout << RED << "WAITING_FOR_PASSWORD" << RESET << std::endl;
			if (message == _password) {
				clientState.state = WAITING_FOR_USERNAME;
				clientState.client.sendClientMsg(client_socket, BOLD "Enter your username: " RESET);
			} else {
				clientState.client.sendClientMsg(client_socket, RED "Wrong password \n\n" RESET);
				clientState.client.sendClientMsg(client_socket, BOLD "Enter Server password: " RESET);
			}
			break;

		case WAITING_FOR_USERNAME:
			std::cout << RED << "WAITING_FOR_USERNAME" << RESET <<  std::endl;
			clientState.username = message;
			clientState.state = WAITING_FOR_NICKNAME;
			clientState.client.sendClientMsg(client_socket, BOLD "Enter your nickname: " RESET);
			break;

		case WAITING_FOR_NICKNAME:
			std::cout << RED << "WAITING_FOR_NICKNAME" << RESET <<  std::endl;
			clientState.nickname = message;
			addUser(clientState.client, clientState.username, clientState.nickname);
			clientState.client.sendClientMsg(client_socket, GREEN "You are now registered! ​✅\n\n" RESET);
			clientState.state = AUTHENTICATED;
			break;

		case AUTHENTICATED:
			std::cout << RED << "AUTHENTICATED" << RESET <<  std::endl;
			// Ici vous pouvez gérer les messages des clients authentifiés
			std::cout << "Client #" << client_socket << " is now authenticated" << std::endl;
			break;
	}
}

void Server::removeClient(int client_socket) {
	close(client_socket);
	_clients.erase(client_socket);

	for (size_t i = 0; i < _fds.size(); ++i) {
		if (_fds[i].fd == client_socket) {
			_fds.erase(_fds.begin() + i);
			break;
		}
	}
}

void Server::startServer() {
	std::cout << bannerServer;
	std::cout << ". . . Listening on port " << _port << " . . . " << std::endl;

	while (true) {
		int poll_count = poll(_fds.data(), _fds.size(), -1);
		if (poll_count == -1) {
			std::cerr << "Error: poll failed" << std::endl;
			break;
		}

		for (size_t i = 0; i < _fds.size(); ++i) {
			if (_fds[i].revents & POLLIN) {
				if (_fds[i].fd == _server_socket) {
					// Nouvelle connexion entrante
					std::cout << RED << "AVANT acceptClient" << RESET <<  std::endl;
					acceptClient();
					std::cout << RED << "APRES acceptClient" << RESET <<  std::endl;
				}
				else {
					// Données provenant d'un client existant
					std::cout << RED << "AVANT handleClientMessage" << RESET <<  std::endl;
					handleClientMessage(_fds[i].fd);
					std::cout << RED << "APRES handleClientMessage" << RESET <<  std::endl;
				}
			}
		}
	}
}
