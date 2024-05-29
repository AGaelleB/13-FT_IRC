#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

Server::Server() : _server_socket(-1), _password("1234"), port(6667) {
}

Server::Server(int port, const std::string &password) : _server_socket(-1), _password(password), port(port) {
	// 1. Crée un socket: socket()
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1) {
		std::cerr << "Error: socket creation failed" << std::endl;
		exit(1);
	}

	// 2. Lie le socket à une adresse IP et un port: bind(), htons()
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_addr.sin_port = htons(port);

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
}

Server::~Server() {
	if (_server_socket != -1)
		close(_server_socket);
}

void Server::handleClientMessage(const std::string& message, Client& client) {
	std::cout << "Client: " << message << std::endl; //suppr

	if (message.substr(0, 6) == "/login")
		std::cout << "Login command received" << std::endl;
	else if (message.substr(0, 8) == "/channel")
		std::cout << "Channel command received" << std::endl;
	else
		std::cout << "Unknown command" << std::endl;
	(void)client;
}

void Server::startServer() {
	socklen_t client_len;
	Client client;

	std::cout << bannerServer;

	std::cout << ". . Listening on port " << port << " . . . " << std::endl;

	while (true) {
		client_len = sizeof(client.getClientAddr());
		int client_socket = accept(_server_socket, (struct sockaddr*)&client.getClientAddr(), &client_len);
		if (client_socket == -1) {
			std::cerr << "Error: connection not accepted" << std::endl;
			continue;
		}

		client.setClientSocket(client_socket);
		std::cout << "\nNew connection accepted ​✅" << std::endl;

		char buffer[1024];
		ssize_t bytes_received;
		
		while ((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0)
		{
			buffer[bytes_received] = '\0';
			std::string message(buffer);
			handleClientMessage(message, client);
		}

		if (bytes_received == -1)
			std::cerr << "Error: data reception failed" << std::endl;
		else
			std::cout << "Client disconnected" << std::endl;

		close(client_socket);
	}
}