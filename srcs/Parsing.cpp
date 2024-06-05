#include "../includes/Server.hpp"

/* 
	// gerer les signaux 
	// voir pq on bloque si client irssi, ne permet pas a nc de se creer 
 */


void Server::parsingDataIrssi(Client &client) {
	std::istringstream stream(this->_irssi_data);
	std::string line;
	std::string nickname;
	std::string username;

	while (std::getline(stream, line)) {
		if (line.find("NICK ") == 0) {
			nickname = line.substr(5);
		}
		else if (line.find("USER ") == 0) {
			std::istringstream userStream(line);
			std::string userKeyword;
			userStream >> userKeyword >> username;
		}
	}
	addUser(client, username, nickname);
	std::cout << YELLOW "\nirssi username = " << username << RESET << std::endl;
	std::cout << YELLOW "irssi nickname = " << nickname << RESET << std::endl;
}

void Server::parsingDataNetclient(Client &client, int new_client_socket) {
	client.setClientSocket(new_client_socket);
	client.sendClientMsg(new_client_socket, bannerIRC);
	client.sendClientMsg(new_client_socket, MSG_WELCOME);
	authenticateAndRegister(client); // Procéder immédiatement à l'authentification et l'inscription
	_clients[new_client_socket] = client; // Ajouter le client à la map des clients après l'authentification
}

void Server::setNonBlocking(int socket) {
	if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error: fcntl F_SETFL failed" << std::endl;
	}
}

void Server::detectClient(int client_socket) {
	setNonBlocking(client_socket);  // Configure le socket en mode non-bloquant
	char buffer[1024];

	// Essayer de recevoir des données initiales sans bloquer
	ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	buffer[bytes_received] = '\0';
	std::string answer(buffer);

	Client& client = _clients[client_socket]; // Accéder à l'objet client par référence

	if (findCapLs(answer) == 0) {
		std::cerr << ORANGE << "connected with irssi!\n" << RESET;
		this->_irssi_data = answer;
		parsingDataIrssi(client); // Passez par référence
		isRegistered(client); 
	}
	else {
		std::cerr << ORANGE << "connected with netcat!\n" << RESET;
		std::cout << BLUE << "\n. . . Waiting for client registration . . . " << RESET << std::endl;
		parsingDataNetclient(client, client_socket); // Passez par référence
		isRegistered(client); 
	}
}


/* 
	pour que ca fonctitonne : 
	1- lanccer irssi sans rien
	2- lancer net cast sans rien
	3- revenir dans irssi et se connect : /connect localhost 6668
	4- revenir dans nc et se connect : nc localhost 6668

 */





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