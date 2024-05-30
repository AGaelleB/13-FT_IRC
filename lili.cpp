#include <cctype>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstring> // Pour strlen

// Fonction pour trim les espaces en début et fin de chaîne
std::string trim(const std::string& str) {
	size_t start = 0;
	while (start < str.size() && std::isspace(str[start])) {
		start++;
	}

	size_t end = str.size();
	while (end > start && std::isspace(str[end - 1])) {
		end--;
	}

	return str.substr(start, end - start);
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
		client.welcomeClient(client_socket);

		char buffer[1024];
		ssize_t bytes_received;

		const char* log_msg = "Please enter the Server password to access\n\n";
		client.sendClientMsg(client_socket, log_msg);
		while (true)
		{
			const char* pass_msg = "Password: ";
			client.sendClientMsg(client_socket, pass_msg);

			bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
			if (bytes_received <= 0) {
				std::cerr << "Error: reception failed" << std::endl;
				break;
			}
			buffer[bytes_received] = '\0';

			std::string pass(buffer);
			pass = trim(pass); // Supprimer les espaces et retours à la ligne

			client.sendClientMsg(client_socket, ("Received: " + pass + "\n").c_str());
			client.sendClientMsg(client_socket, ("Expected: " + this->_password + "\n").c_str());

			if (pass != this->_password) {
				const char* invalid_pass = "Wrong password \n\n";
				client.sendClientMsg(client_socket, invalid_pass);
			} else {
				break;
			}
		}

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
