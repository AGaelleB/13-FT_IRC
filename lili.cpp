#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>



void Server::manage_new_client() {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_socket = accept(_server_socket, (struct sockaddr*)&client_addr, &client_len);
	if (client_socket == -1) {
		std::cerr << "Error: connection not accepted" << std::endl;
		return;
	}

	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	struct pollfd client_fd;
	client_fd.fd = client_socket;
	client_fd.events = POLLIN;
	_fds.push_back(client_fd);
	Client client;
	client.setClientSocket(client_socket);
	client.setClientAddr(client_addr);

	std::cout << "\nNew connection accepted ✅" << std::endl;
	client.sendClientMsg(client_socket, "Welcome to the IRC Server!\n");

	// Authentication
	if (!authenticateClient(client)) {
		close(client_socket);
		return;
	}

	addUser(client, "temp_username", "temp_nickname"); // Placeholder, handle properly later
}

bool Server::authenticateClient(Client &client) {
	char buffer[1024];
	ssize_t bytes_received;

	while (true) {
		const char* pass_msg = "Enter Server password: ";
		client.sendClientMsg(client.getClientSocket(), pass_msg);

		bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
		if (bytes_received <= 0) {
			std::cerr << "Error: reception failed" << std::endl;
			return false;
		}
		buffer[bytes_received] = '\0';

		std::string pass(buffer);
		pass = trim(pass);

		if (pass != this->_password) {
			const char* invalid_pass = "Wrong password \n\n";
			client.sendClientMsg(client.getClientSocket(), invalid_pass);
		} else {
			break;
		}
	}
	client.welcomeClient(client.getClientSocket());
	return true;
}

void Server::manage_new_data(int client_socket) {
	char buffer[1024];
	ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received <= 0) {
		if (bytes_received == 0) {
			std::cout << "Client disconnected ❌" << std::endl;
		} else {
			std::cerr << "Error: data reception failed" << std::endl;
		}
		close(client_socket);
		remove_fd(client_socket);
	} else {
		buffer[bytes_received] = '\0';
		std::string message(buffer);
		std::map<int, Client>::iterator it = _clients.find(client_socket);
		if (it != _clients.end()) {
			it->second.handleClientMsg(message, *this);
		}
	}
}

void Server::remove_fd(int client_socket) {
	for (std::vector<struct pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
		if (it->fd == client_socket) {
			_fds.erase(it);
			break;
		}
	}
	_clients.erase(client_socket);
}

void Server::close_fds() {
	for (std::vector<struct pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
		close(it->fd);
	}
	_fds.clear();
}

void Server::startServer() {
	std::cout << bannerServer;
	// std::cout << "Server <" << _server_socket << "> Connected" << std::endl;
	std::cout << ". . . Listening on _port " << _port << " . . . " << std::endl;

	while (true) {
		if (poll(&_fds[0], _fds.size(), -1) == -1) {
			std::cerr << "Error: poll failed" << std::endl;
			break;
		}

		for (size_t i = 0; i < _fds.size(); ++i) {
			if (_fds[i].revents & POLLIN) { // explain 
				if (_fds[i].fd == _server_socket) // explain 
					manage_new_client(); 
				else
					manage_new_data(_fds[i].fd);
			}
			else if (_fds[i].revents & POLLERR) {// explain 
				std::cerr << "Poll error on fd: " << _fds[i].fd << std::endl;
				close(_fds[i].fd);
				remove_fd(_fds[i].fd);
			}
		}
	}
	close_fds(); // Close all file descriptors when server stops
}
