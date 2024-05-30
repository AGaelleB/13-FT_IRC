#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

/************************************* CONST ET DEST *************************************/

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


/************************************** FUNCTIONS **************************************/

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
	socklen_t client_len;
	Client client;

	std::cout << bannerServer;

	std::cout << ". . . Listening on port " << port << " . . . " << std::endl;

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

		client.sendClientMsg(client_socket, bannerIRC);

		while (true)
		{
			const char* pass_msg = BOLD "Enter Server password: " RESET;
			client.sendClientMsg(client_socket, pass_msg);

			bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
			if (bytes_received <= 0) {
				std::cerr << "Error: reception failed" << std::endl;
				break;
			}
			buffer[bytes_received] = '\0';

			std::string pass(buffer);
			pass = trim(pass);

			// client.sendClientMsg(client_socket, ("Received: " + pass + "\n").c_str());
			// client.sendClientMsg(client_socket, ("Expected: " + this->_password + "\n").c_str());

			if (pass != this->_password) {
				const char* invalid_pass = RED "Wrong password \n\n" RESET;
				client.sendClientMsg(client_socket, invalid_pass);
			}
			else
				break;
		}

		client.welcomeClient(client_socket);

		while ((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0)
		{
			buffer[bytes_received] = '\0';
			std::string message(buffer);
			client.handleClientMsg(message, client);
		}

		if (bytes_received == -1)
			std::cerr << "Error: data reception failed" << std::endl;
		else
			std::cout << "Client disconnected ❌" << std::endl;

		close(client_socket);
	}
}
