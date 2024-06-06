#include "../includes/Server.hpp"

bool Server::isNicknameAvailable(const std::string& nickname) {
	return (_nicknames.find(nickname) == _nicknames.end());
}

void Server::handleClientMessage(int client_fd, Client& client) {
	char buffer[1024];
	ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_received <= 0) {
		if (bytes_received == 0) {
			std::cout << RED << "\nClient " << client.getUser().getNickname() << " is disconnected! ❌ ---> client_socket: " << client_fd << RESET << std::endl;
			std::cout << BOLD << "Total client(s) still online: " << RESET << nfds - 2 << std::endl;
		} else {
			std::cerr << "Error: data reception failed, client_fd: " << client_fd << std::endl;
		}
		close(client_fd);
		_clients.erase(client_fd); // Supprimer le client de la map
		for (int i = 0; i < nfds; ++i) { // Retirer le client de la structure pollfd
			if (fds[i].fd == client_fd) {
				fds[i] = fds[nfds - 1];
				nfds--;
				break;
			}
		}
	}
	else {
		buffer[bytes_received] = '\0';
		std::string message(buffer);
		client.printClientMsg(message, client);
	}
}

void Server::checkPassword(Client &client) {
	char buffer[1024];
	ssize_t bytes_received;

	while (true) {
		client.sendClientMsg(client.getClientSocket(), MSG_PASSWORD);

		// Boucle jusqu'à recevoir des données valides
		while (true) {
			bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
			if (bytes_received == -1 && errno == EWOULDBLOCK) {
				usleep(42); // Attendre un peu avant de réessayer
				continue;
			}
			else if (bytes_received >= 0)
				break;
			else {
				std::cerr << "Error: reception failed during password entry, client_socket: " << client.getClientSocket() << std::endl;
				close(client.getClientSocket());
				return;
			}
		}

		if (bytes_received >= 1023) {
			client.sendClientMsg(client.getClientSocket(), ERROR_PASSWORD_TOO_LONG);

			while ((bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0)) > 0) {
				if (bytes_received < 1023) {
					break;
				}
			}
			continue;
		}

		buffer[bytes_received] = '\0';
		std::string password(buffer);
		password = trim(password);

		if (password != this->_password) {
			std::cout << YELLOW << password << RESET << std::endl; ///
			client.sendClientMsg(client.getClientSocket(), ERROR_PASSWORD);
		}
		else
			break;
	}
}

void Server::addUser(Client &client, const std::string &username, const std::string &nickname) {
	static int current_index = 1;
	User user(current_index++, username, nickname);
	client.setUser(user);
	_clients[client.getClientSocket()] = client;
	_nicknames.insert(nickname);
}

void Server::isRegistered(Client &client) {
	std::stringstream ss;
	ss << GREEN "You are now registered! ✅ ---> client_socket: " << client.getClientSocket() << RESET << std::endl;
	std::string registeredMsg = ss.str();
	client.sendClientMsg(client.getClientSocket(), registeredMsg.c_str());

	std::cout << GREEN << "\nClient " << client.getUser().getNickname() << " is registered! ✅ ---> : " << client.getClientSocket() << RESET << std::endl;
}

void Server::authenticateAndRegister(Client &client) {
	std::string username;
	std::string nickname;

	checkPassword(client);
	username = client.setUserName();
	nickname = client.setNickName(*this);
	addUser(client, username, nickname);

	std::cout << YELLOW "\nnetcat username = " << username << RESET << std::endl;
	std::cout << YELLOW "netcat nickname = " << nickname << RESET << std::endl;
}