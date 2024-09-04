#include "../../includes/Server.hpp"

void Server::checkPassword(Client &client) {
    char buffer[1024];
    ssize_t bytes_received;

    client.sendClientMsg(client.getClientSocket(), MSG_PASSWORD);

    while (true) {
        // Vérifier l'état du client en utilisant poll_count
        if (poll_count <= 0) {
            std::cerr << "Poll error or timeout during password entry." << std::endl;
            handleClientDisconnection(client.getClientSocket());
            return;
        }

        bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cerr << RED << "Client disconnected during password entry, client_socket: " << client.getClientSocket() << RESET << std::endl;
            handleClientDisconnection(client.getClientSocket());
            return;
        }

        buffer[bytes_received] = '\0';
        std::string password(trim(buffer));

        if (password != this->_password) {
            client.sendClientMsg(client.getClientSocket(), ERROR_PASSWORD);
        } else {
            break;
        }
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

void Server::removeClientFromPoll(int client_fd) {
    for (int i = 0; i < nfds; i++) {
        if (fds[i].fd == client_fd) {
            for (int j = i; j < nfds - 1; j++) {
                fds[j] = fds[j + 1];
            }
            nfds--;
            break;
        }
    }
}

void Server::handleClientDisconnection(int client_fd) {
    close(client_fd);
    removeClientFromPoll(client_fd);
}


void Server::authenticateAndRegister(Client &client) {
	std::string username;
	std::string nickname;

	checkPassword(client);
	username = client.setUserName();
	nickname = client.setNickName(*this);
	addUser(client, username, nickname);
}
