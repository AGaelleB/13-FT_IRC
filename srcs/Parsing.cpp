#include "../includes/Server.hpp"

/* 
	CAP LS
	NICK gaelle
	USER gaelle gaelle localhost :Gaga Coding
 */


void Server::parsingDataIrssi(Client client) {
    std::cerr << "Contents of _irssi_data before parsing:\n" << this->_irssi_data << std::endl;

    std::istringstream stream(this->_irssi_data);
    std::string line;
    std::string nickname;
    std::string username;

    while (std::getline(stream, line)) {
        if (line.find("NICK ") == 0) {
            nickname = line.substr(5);
            std::cerr << "Trimmed Nickname: " << nickname << std::endl;
        } else if (line.find("USER ") == 0) {
            std::istringstream userStream(line);
            std::string userKeyword;
            userStream >> userKeyword >> username;
            std::cerr << "Trimmed Username: " << username << std::endl;
        }
    }
    addUser(client, username, nickname);
    std::cerr << "Nickname after addUser: " << client.getUser().getNickname() << std::endl;
    std::cerr << "Username after addUser: " << client.getUser().getUsername() << std::endl;
}


void Server::parsingDataNetclient(Client client, int new_client_socket) {

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
	char buffer[1024];
	sleep(1);
	ssize_t bytes_received = recv(new_client_socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received > 0) {
		buffer[bytes_received] = '\0';
		std::string answer(buffer);

		if (findCapLs(answer) == 0) {
   			 std::cerr << "Contents of answer:\n" << answer << std::endl;
			std::cerr << "connected with irssi!\n\n";
			this->_irssi_data = answer;
			parsingDataIrssi(client);
			std::cerr << "END connection with irssi!\n\n";
		}
		else { 
			std::cerr << "connected with netcat!\n\n";
			std::cout << BLUE << "\n. . . Waiting for client registration . . . " << RESET << std::endl;
			parsingDataNetclient(client, new_client_socket);
		}
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