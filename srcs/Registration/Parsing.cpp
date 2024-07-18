#include "../../includes/Server.hpp"

void Server::handleClientMessage(int client_fd, Client& client) {
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer)); // clear the buffer
	ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_received <= 0) {
		if (bytes_received == 0) {
			std::cout << RED << "\nClient " << client.getUser().getNickname() << " is disconnected! ❌ [socket: " << client_fd << "]" << RESET << std::endl;
		}
		else {
			std::cerr << "Error: data reception failed [socket: " << client_fd << "]" << std::endl;
			std::cerr << "recv error: " << strerror(errno) << " (errno: " << errno << ")" << std::endl;
		}

		if (!client.getUser().getNickname().empty())
			removeNickname(client.getUser().getNickname());

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
		if (static_cast<size_t>(bytes_received) >= sizeof(buffer) - 1) {
			std::string netcatMessage = "Error: Command too long\n";
			std::string irssiMessage = ":localhost 400 " + client.getUser().getNickname() + " :Command too long\r\n";
			sendErrorMessage(client, netcatMessage, irssiMessage);
			memset(buffer, 0, sizeof(buffer)); // clear the buffer
			return;
		}

		buffer[bytes_received] = '\0';
		std::string message(buffer);

		if (message.size() > MAX_TOPIC_SIZE) {
			std::string netcatMessage = "Error: Message too long\n";
			std::string irssiMessage = ":localhost 400 " + client.getUser().getNickname() + " :Message too long\r\n";
			sendErrorMessage(client, netcatMessage, irssiMessage);
			memset(buffer, 0, sizeof(buffer)); // clear the buffer
			return;
		}

		std::vector<std::string> tokens = split(message);
		if (tokens.empty()) {
			std::string netcatMessage = "Error: Unknown command\n";
			std::string irssiMessage = ERR_UNKNOWNCOMMAND(client.getUser().getNickname(), message);
			sendErrorMessage(client, netcatMessage, irssiMessage);
			memset(buffer, 0, sizeof(buffer)); // clear the buffer
			return;
		}

		parseClientMsg(message, client);
	}
}

void Server::logRPLirssi(Client& client) {

	client.sendClientMsg(client.getClientSocket(), bannerIRC);
	client.sendClientMsg(client.getClientSocket(), "\n");

	std::string IPaddress = "127.0.0.1";
	std::string welcomeMsg = RPL_WELCOME(client.getUser().getNickname(), IPaddress);
	std::string yourHostMsg = RPL_YOURHOST(client.getUser().getNickname(), "localhost");
	std::string createdMsg = RPL_CREATED(client.getUser().getNickname(), "now");
	std::string myInfoMsg = RPL_MYINFO(client.getUser().getNickname(), "localhost");
	std::string iSupportMsg = RPL_ISUPPORT(client.getUser().getNickname());

	client.sendClientMsg(client.getClientSocket(), welcomeMsg.c_str());
	client.sendClientMsg(client.getClientSocket(), yourHostMsg.c_str());
	client.sendClientMsg(client.getClientSocket(), createdMsg.c_str());
	client.sendClientMsg(client.getClientSocket(), myInfoMsg.c_str());
	client.sendClientMsg(client.getClientSocket(), iSupportMsg.c_str());
}

void Server::parsingDataIrssi(Client &client, int new_client_socket) {
	std::istringstream	stream(this->_irssi_data);
	std::string			line;
	std::string			nickname;
	std::string			username;

	client.setClientSocket(new_client_socket);
	while (std::getline(stream, line)) {
		if (line.find("NICK ") == 0) {
			nickname = line.substr(5);
			nickname = trim(nickname);
		}
		else if (line.find("USER ") == 0) {
			std::istringstream userStream(line);
			std::string userKeyword;
			userStream >> userKeyword >> username;
			username = trim(username);
		}
	}
	addUser(client, username, nickname);
	logRPLirssi(client);
}

bool Server::parsingDataNetcat(Client &client, int new_client_socket) {
    client.setClientSocket(new_client_socket);
    client.sendClientMsg(new_client_socket, bannerIRC);
    client.sendClientMsg(new_client_socket, MSG_WELCOME);
    client.sendClientMsg(new_client_socket, MSG_HELP_CLIENT);
    
    try {
        authenticateAndRegister(client);
        _clients[new_client_socket] = client; // Ajout du client à la map
        return true;
    }
	catch (const std::exception& e) {
        std::cerr << "Error during authentication and registration: " << e.what() << std::endl;
        return false;
    }
}

void Server::detectClient(int client_socket) {
	char buffer[1024] = {0};
	bool data_received = false;
	const int max_attempts = 10;
	const int delay_between_attempts = 5000;

	for (int attempt = 0; attempt < max_attempts; ++attempt) {
		usleep(delay_between_attempts);
		ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
		if (bytes_received > 0) {
			buffer[bytes_received] = '\0';
			data_received = true;
			break;
		}
		else if (bytes_received == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
			std::cerr << RED << "Error receiving data\n" << RESET;
			return;
		}
	}

	Client& client = _clients[client_socket]; // Accéder à l'objet client par référence
	if (data_received) {
		std::string answer(buffer);
		if (findCapLs(answer) == 0) {
			if (checkPasswordirssi(answer, client) == 1) {
				this->_irssi_data = answer;
				parsingDataIrssi(client, client_socket);
				isRegistered(client);
				std::cerr << ORANGE << "1 - [" << client.getUser().getNickname() << "] is connected with irssi!\n" << RESET;
				client.isIrssi = true;
				std::cout << GREEN << "\n1 - New connection accepted! ✅ [socket: " << client.getClientSocket() << "]" << RESET << std::endl;
			}
			else {
				client.sendClientMsg(client_socket, ERROR_ARGS_IRSSI);
				close(client_socket);
				_clients.erase(client_socket);
			}
		}
	}
	else {
		std::cout << BLUE << "\n. . . Waiting for client registration . . . " << RESET << std::endl;
		parsingDataNetcat(client, client_socket);
		isRegistered(client); 
		std::cerr << ORANGE << "2 - [" << client.getUser().getNickname() << "] is connected with netcat!\n" << RESET;
		std::cout << GREEN << "\n2- New connection accepted! ✅ [socket: " << client.getClientSocket() << "]" << RESET << std::endl;
	}
}


// /connect localhost 6667 1


