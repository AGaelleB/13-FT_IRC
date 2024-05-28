#include "../includes/Server.hpp"

Server::Server() : _server_socket(-1), _password("1234"), port(6667) {
}

Server::Server(int port, const std::string &password) : _server_socket(-1), _password(password), port(port) {
	// 1. Crée un socket: socket()
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1) {
		std::cerr << "Erreur lors de la création du socket" << std::endl;
		exit(1);
	}

	// 2. Lie le socket à une adresse IP et un port: bind(), htons()
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_addr.sin_port = htons(port);

	if (bind(_server_socket, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) {
		std::cerr << "Erreur lors du bind" << std::endl;
		close(_server_socket);
		exit(1);
	}

	// 3. Met le socket en écoute pour les connexions entrantes: listen()
	if (listen(_server_socket, SOMAXCONN) == -1) {
		std::cerr << "Erreur lors du listen" << std::endl;
		close(_server_socket);
		exit(1);
	}
}

Server::~Server() {
	if (_server_socket != -1)
		close(_server_socket);
}

void Server::startServer() {
	int client_socket;
	socklen_t client_len;
	struct sockaddr_in client_addr;

	std::cout << "Serveur en écoute sur le port " << port << std::endl;

	// 4. Accepte les connexions des clients: accept()
	while (true) {
		client_len = sizeof(client_addr);
		client_socket = accept(_server_socket, (struct sockaddr*)&client_addr, &client_len);
		if (client_socket == -1) {
			std::cerr << "Erreur lors de l'acceptation de la connexion" << std::endl;
			continue;
		}

		std::cout << "Nouvelle connexion acceptée" << std::endl;

		// Vous pouvez ajouter ici le code pour traiter les données reçues du client.

		// Fermez le socket client après avoir traité les données.
		close(client_socket);
	}
}
