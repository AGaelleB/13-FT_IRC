#include "../../includes/Server.hpp"

void Server::checkPassword(Client &client) {
	char buffer[1024];
	ssize_t bytes_received;

	while (true) {
		client.sendClientMsg(client.getClientSocket(), MSG_PASSWORD);

		// Boucle jusqu'à recevoir des données valides
		while (true) {
			bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
			if (bytes_received == -1) {
				if (errno == EWOULDBLOCK) {
					usleep(42);
					continue;
				}
				else {
					std::cerr << "Error: reception failed during password entry, client_socket: " << client.getClientSocket() << std::endl;
					close(client.getClientSocket());
					return;
				}
			}
			else if (bytes_received == 0) {
				// Le client a fermé la connexion
				std::cerr << RED << "\nClient disconnected during password entry, client_socket: " << client.getClientSocket() << RESET << std::endl << std::endl;
				close(client.getClientSocket());
				return;
			}
			else
				break;
		}

		if (bytes_received >= 1023) {
			client.sendClientMsg(client.getClientSocket(), ERROR_PASSWORD_TOO_LONG);

			while ((bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0)) > 0) {
				if (bytes_received < 1023)
					break;
			}
			continue;
		}

		buffer[bytes_received] = '\0';
		std::string password(buffer);
		password = trim(password);

		if (password != this->_password)
			client.sendClientMsg(client.getClientSocket(), ERROR_PASSWORD);
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

bool Server::isRegistered(Client &client) {
	return !client.getUser().getNickname().empty() && !client.getUser().getUsername().empty();
}

void Server::authenticateAndRegister(Client &client) {
	std::string username;
	std::string nickname;

	checkPassword(client);
	username = client.setUserName();
	nickname = client.setNickName(*this);
	addUser(client, username, nickname);
}
