#include "../includes/Server.hpp"

/* 
	// gerer les signaux 
	// voir pq on bloque si client irssi, ne permet pas a nc de se creer 
 */


void Server::parsingDataIrssi(Client client) {
    std::istringstream stream(this->_irssi_data);
    std::string line;
    std::string nickname;
    std::string username;

    while (std::getline(stream, line)) {
        if (line.find("NICK ") == 0) {
            nickname = line.substr(5);
        } else if (line.find("USER ") == 0) {
            std::istringstream userStream(line);
            std::string userKeyword;
            userStream >> userKeyword >> username;
        }
    }
    addUser(client, username, nickname);
}

void Server::parsingDataNetclient(Client client, int new_client_socket) {
	std::cout << YELLOW "~~~ parsingDataNetclient ~~~" << RESET << std::endl;

	// Ajouter le nouveau client au vecteur pollfd
	fds[nfds].fd = new_client_socket;
	fds[nfds].events = POLLIN;
	nfds++;
	client.setClientSocket(new_client_socket);
	client.sendClientMsg(new_client_socket, bannerIRC);
	client.sendClientMsg(new_client_socket, MSG_WELCOME);
	authenticateAndRegister(client);
	_clients[new_client_socket] = client; // Ajouter le client au map des clients après l'authentification

}

void Server::detectClient(Client client, int new_client_socket) {
	
	std::cout << YELLOW "~~~ detectClient ~~~" << RESET << std::endl;
	
	char buffer[1024];

	std::cout << YELLOW "~~~ 1 ~~~" << RESET << std::endl;
	sleep(1);

	std::cout << YELLOW "new_client_socket: " << new_client_socket << RESET << std::endl;
	std::cout << YELLOW "buffer: " << buffer << RESET << std::endl;

	ssize_t bytes_received = recv(new_client_socket, buffer, sizeof(buffer) - 1, 0);
	
	std::cout << YELLOW "bytes_received: " << bytes_received << RESET << std::endl;
	
	if (bytes_received > 0) {
		buffer[bytes_received] = '\0';
		std::string answer(buffer);
		
		std::cout << YELLOW "answer: " << answer << RESET << std::endl;
		if (findCapLs(answer) == 0) {
			std::cerr << "connected with irssi!\n\n";
			this->_irssi_data = answer;
			parsingDataIrssi(client);
		}
		else { 
			std::cerr << "connected with netcat!\n\n";
			std::cout << BLUE << "\n. . . Waiting for client registration . . . " << RESET << std::endl;
			parsingDataNetclient(client, new_client_socket);
		}
		isRegistered(client);
	}
	else {
		std::cerr << "Error: recv failed" << std::endl;
		close(new_client_socket);
	}
}



// void Client::parsingCmd(const std::string& message, Client& client) {
// 	std::cout << BOLD << "\n" << client.getUser().getUsername()  <<  " msg: " << RESET << message;

// 	if (message.substr(0, 6) == "/login")
// 		std::cout << "Login command received" << std::endl;
// 	// else if (message.substr(0, 8) == "/channel")
// 	// 	std::cout << "Channel command received" << std::endl;
// 	else {
// 		// const char* unknownCommand = "Unknown command ❌\n";
// 		sendClientMsg(this->getClientSocket(), unknownCommand);
// 	}
// 	(void)client;
// }