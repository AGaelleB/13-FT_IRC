#include "../../includes/Server.hpp"

bool			Server::_shutdown_signal = false;
struct pollfd	fds[1024];
int				nfds = 1;
const int		_MAX_CLIENTS = 20;


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

	// Configurer l'option SO_REUSEADDR pour pas bloquer le port
	int opt = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		std::cerr << "Error: setsockopt SO_REUSEADDR failed" << std::endl;
		close(_server_socket);
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

void Server::initializeServer() {
	std::cout << bannerServer;
	std::cout << BLUE << ". . . Listening on port " << _port << " . . . " << RESET << std::endl;

	signal(SIGINT, Server::SignalHandler); // catch the signal (ctrl + c)
	signal(SIGQUIT, Server::SignalHandler); // catch the signal (ctrl + \)
}

void Server::acceptNewConnection() {
	if (_clients.size() >= _MAX_CLIENTS) {
		std::cerr << ERROR_MAX_CLIENT_SERVER << std::endl;
		int new_client_socket = accept(_server_socket, NULL, NULL);
		if (new_client_socket != -1) {
			Client tempClient;
			tempClient.sendClientMsg(new_client_socket, ERROR_MAX_CLIENT);
			close(new_client_socket);
		}
		return;
	}

	Client client;
	socklen_t client_len = sizeof(client.getClientAddr());
	int new_client_socket = accept(_server_socket, (struct sockaddr*)&client.getClientAddr(), &client_len);
	if (new_client_socket == -1) {
		std::cerr << "Error: connection not accepted" << std::endl;
		return;
	}

	std::cout << GREEN << "\nNew connection accepted! ✅ [socket: " << new_client_socket << "]" << RESET << std::endl;
	std::cout << BOLD << "Total client(s) online: " << RESET << nfds << "/" << _MAX_CLIENTS << std::endl;

	fds[nfds].fd = new_client_socket;
	fds[nfds].events = POLLIN;
	
	nfds++;
	_clients[new_client_socket] = client;
	detectClient(new_client_socket);
}



void Server::handlePollEvents() {
	for (int i = 0; i < nfds; ++i) {
		if (fds[i].revents & POLLIN) {
			if (fds[i].fd == _server_socket)
				acceptNewConnection();
			else
				handleClientMessage(fds[i].fd, _clients[fds[i].fd]);
		}
	}
}

void Server::stopServer() {
	std::cout << "Shutting down server..." << std::endl;

	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		send(it->first, MSG_DISCONNECT, strlen(MSG_DISCONNECT), 0);
		close(it->first);
	}
	close(_server_socket);
	std::cout << "Server stopped" << std::endl;
}

void Server::startServer() {
	initializeServer();

	while (!_shutdown_signal) {
		int poll_count = poll(fds, nfds, 1000);
		if (poll_count == -1) {
			if (errno == EINTR)
				continue; // Interrompu par un signal
			std::cerr << "Error: poll failed" << std::endl;
			break;
		}
		handlePollEvents();
	}
	stopServer();
}





