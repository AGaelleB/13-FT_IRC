#include "../includes/Server.hpp"

struct pollfd	fds[1024];
int				nfds = 1;


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
					client.sendClientMsg(client_socket, MSG_WELCOME);
					authenticateAndRegister(client);
					_clients[client_socket] = client; // Ajouter le client au map des clients après l'authentification
					
				}
 				else {
					std::map<int, Client>::iterator it = _clients.find(fds[i].fd);
					if (it != _clients.end())
						handleClientMessage(fds[i].fd, it->second);
					else
						std::cerr << "Error: Client not found in _clients map, client_fd: " << fds[i].fd << std::endl;
				}
			}
		}
	}
}