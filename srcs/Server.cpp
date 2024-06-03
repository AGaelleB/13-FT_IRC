#include "../includes/Server.hpp"

struct pollfd fds[1024];
int nfds = 1;

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

	// Initialiser la structure pollfd pour le serveur
		fds[0].fd = _server_socket;
		fds[0].events = POLLIN;  // Surveiller les événements d'entrée
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

void Server::startServer() {
	std::cout << bannerServer;
	std::cout << BLUE << ". . . Listening on port " << _port << " . . . " << RESET << std::endl;

	while (true) {
		int poll_count = poll(fds, nfds, -1);
		if (poll_count == -1) {
			std::cerr << "Error: poll failed" << std::endl;
			exit(1);
		}


		for (int i = 0; i < nfds; ++i) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == _server_socket) {
					// Nouvelle connexion
					Client client;
					socklen_t client_len = sizeof(client.getClientAddr());
					int client_socket = accept(_server_socket, (struct sockaddr*)&client.getClientAddr(), &client_len);
					if (client_socket == -1) {
						std::cerr << "Error: connection not accepted" << std::endl;
						continue;
					}

					std::cout << GREEN << "\nNew connection accepted! ✅ ---> client_socket: " << client_socket << RESET << std::endl;
					std::cout << "Total clients: " << nfds << std::endl;
					std::cout << BLUE << "\n. . . Waiting for client registration . . . " << RESET << std::endl;

					// Ajouter le nouveau client au vecteur pollfd
					fds[nfds].fd = client_socket;
					fds[nfds].events = POLLIN;
					nfds++;

					client.setClientSocket(client_socket);

					client.sendClientMsg(client_socket, bannerIRC);
					client.welcomeClient(client_socket);

					// Gérer l'authentification et les informations utilisateur
					authenticateAndRegister(client);

					// Ajouter le client au map des clients après l'authentification
					_clients[client_socket] = client;

					// std::cout << "Client registration complete. Connection remains open for client_socket: " << client_socket << std::endl << std::endl;
				} else {
					// Message reçu d'un client existant
					// std::cout << "Handling message from client_fd: " << fds[i].fd << std::endl;
					handleClientMessage(fds[i].fd);
				}
			}
		}
	}
}

void Server::checkPassword(Client &client) {
	char buffer[1024];
	ssize_t bytes_received;

	while (true) {
		const char* passwordMsg = BOLD "Enter Server password: " RESET;
		client.sendClientMsg(client.getClientSocket(), passwordMsg);

		bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
		if (bytes_received <= 0) {
			std::cerr << "Error: reception failed during password entry, client_socket: " << client.getClientSocket() << std::endl;
			close(client.getClientSocket());
			return;
		}

		buffer[bytes_received] = '\0';
		std::string password(buffer);
		password = trim(password);

		if (password != this->_password) {
			const char* wrongPass = RED "Wrong password \n\n" RESET;
			client.sendClientMsg(client.getClientSocket(), wrongPass);
		}
		else
			break;
	}
}

void Server::authenticateAndRegister(Client &client) {
	std::string username;
	std::string nickname;

	checkPassword(client);
	username = client.setUserName();
	nickname = client.setNickName();
	addUser(client, username, nickname);

    std::stringstream ss;
    ss << GREEN "You are now registered! ✅ ---> client_socket: " << client.getClientSocket() << RESET << std::endl;
    std::string registeredMsg = ss.str();
    client.sendClientMsg(client.getClientSocket(), registeredMsg.c_str());

    std::cout << GREEN << "\nClient " << username << " is registered! ✅ ---> client_socket: " << client.getClientSocket() << RESET << std::endl;
}

void Server::handleClientMessage(int client_fd) {
	char buffer[1024];
	ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_received <= 0) {
		if (bytes_received == 0) {
			std::cout << "\nClient disconnected ❌, client_fd: " << client_fd << std::endl;
			std::cout << "Total clients: " << nfds - 2 << std::endl;
		}
		else
			std::cerr << "Error: data reception failed, client_fd: " << client_fd << std::endl;
		close(client_fd);
		_clients.erase(client_fd); // Supprimer le client de la map
		// Retirer le client de la structure pollfd
		for (int i = 0; i < nfds; ++i) {
			if (fds[i].fd == client_fd) {
				fds[i] = fds[nfds - 1];
				nfds--;
				break;
			}
		}
	} else {
		buffer[bytes_received] = '\0';
		std::string message(buffer);
		// std::cout << "Received message from client_fd " << client_fd << ": " << message << std::endl;
		_clients[client_fd].handleClientMsg(message, _clients[client_fd]);
	}
}
