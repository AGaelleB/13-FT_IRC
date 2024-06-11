#include "../../includes/Server.hpp"

// Définition de la variable statique shutdown_signal
bool Server::shutdown_signal = false;

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

void Server::sendDisconnectMessageToClients() {
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		send(it->first, MSG_DISCONNECT, strlen(MSG_DISCONNECT), 0);
	}
}

void Server::stopServer() {
	std::cout << "Shutting down server..." << std::endl;
	sendDisconnectMessageToClients();
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		close(it->first);
	close(_server_socket);
	std::cout << "Server stopped." << std::endl;
}

void Server::startServer() {
	std::cout << bannerServer;
	std::cout << BLUE << ". . . Listening on port " << _port << " . . . " << RESET << std::endl;
	// std::cout << MSG_HELP_SERVER << std::endl;  // test

	signal(SIGINT, Server::SignalHandler); // catch the signal (ctrl + c)
	signal(SIGQUIT, Server::SignalHandler); // catch the signal (ctrl + \)

	while (!shutdown_signal) {
		int poll_count = poll(fds, nfds, 1000);
		if (poll_count == -1) {
			if (errno == EINTR)
				continue; // Interrompu par un signal
			std::cerr << "Error: poll failed" << std::endl;
			break;
		}
		for (int i = 0; i < nfds; ++i) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == _server_socket) {
					// Nouvelle connexion
					Client client;
					socklen_t client_len = sizeof(client.getClientAddr());
					int new_client_socket = accept(_server_socket, (struct sockaddr*)&client.getClientAddr(), &client_len);
					if (new_client_socket == -1) {
						std::cerr << "Error: connection not accepted" << std::endl;
						continue;
					}
					std::cout << GREEN << "\nNew connection accepted! ✅ ---> client_socket: " << new_client_socket << RESET << std::endl;
					std::cout << BOLD << "Total client(s) online: " << RESET << nfds << std::endl;
					fds[nfds].fd = new_client_socket;
					fds[nfds].events = POLLIN;
					nfds++;
					_clients[new_client_socket] = client;
					detectClient(new_client_socket);
				}
				else
					handleClientMessage(fds[i].fd, _clients[fds[i].fd]);
			}
		}
	}
	stopServer();
}


/* 

[(status)] /connect localhost 6667
	Irssi v1.2.3-1ubuntu4 - https://irssi.org                                                                                                                                                      
	15:37 -!-  ___           _
	15:37 -!- |_ _|_ _ _____(_)
	15:37 -!-  | || '_(_-<_-< |
	15:37 -!- |___|_| /__/__/_|
	15:37 -!- Irssi v1.2.3-1ubuntu4 - https://irssi.org
	15:38 -!- Irssi: Looking up localhost
	15:38 -!- Irssi: Connecting to localhost [127.0.0.1] port 6667
	15:38 -!- Irssi: Connection to localhost established
	15:38 -!- add [-w <password>] Not enough parameters.


[(status)] /connect localhost 6667 1
	15:38 [localhost2] -!- Irssi: Looking up localhost
	15:38 [localhost2] -!- Irssi: Connecting to localhost [127.0.0.1] port 6667
	15:38 [localhost2] -!- Irssi: Connection to localhost established
	15:38 [localhost2] -!- !!! WELCOME TO THE INTERNET RELAY NETWORK !!! | gaelle:127.0.0.1
	15:38 [localhost2] -!- -Your host is [pirate server] (localhost), running version 1.0.1 
	15:38 [localhost2] -!- -This server was created 07-06-2024 15:37:52
	15:38 [localhost2] -!- -[pirate server] 1.0.1 io kost k
	15:38 [localhost2] -!- -CHANNELLEN=32 NICKLEN=9 TOPICLEN=307 : are supported by this server
	15:38 [localhost2] -!- I /$$$$$$$$ /$$$$$$$$     /$$$$$$ /$$$$$$$   /$$$$$$ 
	15:38 [localhost2] -!- | $$_____/|__  $$__/    |_  $$_/| $$__  $$ /$$__  $$
	15:38 [localhost2] -!- | $$         | $$         | $$  | $$  \ $$| $$  \__/
	15:38 [localhost2] -!- | $$$$$      | $$         | $$  | $$$$$$$/|$$      
	15:38 [localhost2] -!- | $$__/      | $$         | $$  | $$__  $$|$$      
	15:38 [localhost2] -!- | $$         | $$         | $$  | $$  \ $$| $$    $$
	15:38 [localhost2] -!- | $$         | $$        /$$$$$$| $$  | $$|  $$$$$$/
	15:38 [localhost2] -!- |__/         |__//$$$$$$|______/|__/  |__/ \______/ 
	15:38 [localhost2] -!-                 |______/                          
	15:38 [localhost2] -!- Enjoy!
	15:38 [localhost2] -!- :gaelle MODE gaelle :+i

 */