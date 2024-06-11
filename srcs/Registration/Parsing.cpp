#include "../../includes/Server.hpp"

void Server::handleClientMessage(int client_fd, Client& client) {
	char buffer[1024];
	ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_received <= 0) {
		if (bytes_received == 0) {
			std::cout << RED << "\nClient " << client.getUser().getNickname() << " is disconnected! ❌ ---> client_socket: " << client_fd << RESET << std::endl;
			std::cout << BOLD << "Total client(s) still online: " << RESET << nfds - 2 << std::endl;
		}
		else
			std::cerr << "Error: data reception failed, client_fd: " << client_fd << std::endl;
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
	client.setClientSocket(new_client_socket);
	
	logRPLirssi(client);

	std::cout << YELLOW "\nirssi username = " << username << RESET << std::endl;
	std::cout << YELLOW "irssi nickname = " << nickname << RESET << std::endl;
}

void Server::parsingDataNetcat(Client &client, int new_client_socket) {
	client.setClientSocket(new_client_socket);
	client.sendClientMsg(new_client_socket, bannerIRC);
	client.sendClientMsg(new_client_socket, MSG_WELCOME);
	client.sendClientMsg(new_client_socket, MSG_HELP_CLIENT);
	authenticateAndRegister(client);
	_clients[new_client_socket] = client; // Ajout du client à la map
}

void Server::setNonBlocking(int socket) {
	if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error: fcntl F_SETFL failed" << std::endl;
	}
}

void Server::detectClient(int client_socket) {
	setNonBlocking(client_socket);  // socket en mode non-bloquant
	char buffer[1024] = {0};

	usleep(42);
	ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	buffer[bytes_received] = '\0';
	std::string answer(buffer);
	   
	// std::cout << YELLOW << "DetectClient received: " << answer << RESET << std::endl; // test

	Client& client = _clients[client_socket]; // Accéder à l'objet client par référence
	if (findCapLs(answer) == 0) {
		if (checkPasswordirssi(answer, client) == 1) {
			std::cerr << ORANGE << "connected with irssi!\n" << RESET;
			this->_irssi_data = answer;
			parsingDataIrssi(client, client_socket);
			isRegistered(client);
		}
		else {
			std::cout << RED << "Error: must be ./Server <port> <password>" << RESET << std::endl;
			std::string err_needmoreparams = ERR_NEEDMOREPARAMS(client.getUser().getNickname(), "add password");
			client.sendClientMsg(client.getClientSocket(), err_needmoreparams.c_str());
		}
	}
	else {
		std::cerr << ORANGE << "connected with netcat!\n" << RESET;
		std::cout << BLUE << "\n. . . Waiting for client registration . . . " << RESET << std::endl;
		parsingDataNetcat(client, client_socket);
		isRegistered(client); 
	}
}


/* 
/msg gaga coucou

/connect localhost 6667
nc localhost 6667
 */

