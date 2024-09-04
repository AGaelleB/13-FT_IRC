#include "../../includes/Server.hpp"

void Server::checkPassword(Client &client) {
    char buffer[1024];
    ssize_t bytes_received;

    client.sendClientMsg(client.getClientSocket(), MSG_PASSWORD);

    while (true) {
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

void Server::handleClientDisconnection(int client_fd) {
    std::cout << RED << "Client disconnected! [socket: " << client_fd << "]" << RESET << std::endl;

    // Supprimer le client de la map
    _clients.erase(client_fd);

    // Retirer le client de la structure pollfd
    for (int i = 0; i < nfds; ++i) {
        if (fds[i].fd == client_fd) {
            fds[i] = fds[nfds - 1]; // Remplace l'entrée courante par la dernière
            nfds--;
            break;
        }
    }

    close(client_fd);
}

void Server::authenticateAndRegister(Client &client) {
    try {
        checkPassword(client);
        std::string username = client.setUserName();
        std::string nickname = client.setNickName(*this);
        addUser(client, username, nickname);
    } catch (const std::exception& e) {
        std::cerr << "Error during authentication and registration: " << e.what() << std::endl;
        handleClientDisconnection(client.getClientSocket());
    }
}