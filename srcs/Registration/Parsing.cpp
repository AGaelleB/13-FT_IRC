#include "../../includes/Server.hpp"

void Server::handleClientMessage(int client_fd, Client& client) {
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer)); 
	ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_received <= 0) {
		if (bytes_received == 0)
			std::cout << RED << "\nClient " << client.getUser().getNickname() << " is disconnected! ❌ [socket: " << client_fd << "]" << RESET << std::endl;
		else 
			std::cerr << "Error: data reception failed [socket: " << client_fd << "]" << std::endl;

		if (!client.getUser().getNickname().empty())
			removeNickname(client.getUser().getNickname());

		partAllChannels(client);
		close(client_fd);
		_clients.erase(client_fd);
		for (int i = 0; i < nfds; ++i) { 
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
			memset(buffer, 0, sizeof(buffer));
			return;
		}

		buffer[bytes_received] = '\0';
		std::string message(buffer);

		if (message.size() > MAX_TOPIC_SIZE) {
			std::string netcatMessage = "Error: Message too long\n";
			std::string irssiMessage = ":localhost 400 " + client.getUser().getNickname() + " :Message too long\r\n";
			sendErrorMessage(client, netcatMessage, irssiMessage);
			memset(buffer, 0, sizeof(buffer)); 
			return;
		}

		std::vector<std::string> tokens = split(message);
		if (tokens.empty()) {
			std::string netcatMessage = "Error: Unknown command\n";
			std::string irssiMessage = ERR_UNKNOWNCOMMAND(client.getUser().getNickname(), message);
			sendErrorMessage(client, netcatMessage, irssiMessage);
			memset(buffer, 0, sizeof(buffer)); 
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
		_clients[new_client_socket] = client;
		return (true);
	}
	catch (const std::exception& e) {
		std::cerr << "Error during authentication and registration: " << e.what() << std::endl;
		return (false);
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
	}
	Client& client = _clients[client_socket];
	if (data_received) {
		std::string answer(buffer);
		if (findCapLs(answer) == 0) {
			if (checkPasswordirssi(answer, client) == 1) {
				this->_irssi_data = answer;
				parsingDataIrssi(client, client_socket);
				if (isRegistered(client)) {
					client.isIrssi = true;
					std::cout << GREEN << "\nNew connection accepted! ✅ [socket: " << client.getClientSocket() << "]" << RESET << std::endl;
					std::cout << GREEN << "<" << client.getUser().getNickname() << "> is connected with irssi!\n" << RESET;
				}
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
		if (parsingDataNetcat(client, client_socket)) {
			if (isRegistered(client)) {
				std::cout << GREEN << "\nNew connection accepted! ✅ [socket: " << client.getClientSocket() << "]" << RESET << std::endl;
				std::cerr << GREEN << "<" << client.getUser().getNickname() << "> is connected with netcat!\n" << RESET;
			}
			else {
				std::cerr << RED << "Client disconnected during registration process ❌ [socket: " << client.getClientSocket() << "]" << RESET << std::endl;
				close(client_socket);
				_clients.erase(client_socket);
			}
		}
		else {
			std::cerr << RED << "Client disconnected during registration process ❌ [socket: " << client.getClientSocket() << "]" << RESET << std::endl;
			close(client_socket);
			_clients.erase(client_socket);
		}
	}
}
