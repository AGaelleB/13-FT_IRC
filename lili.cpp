void Server::detectClient(int client_socket) {

	setNonBlocking(client_socket);  // Configure le socket en mode non-bloquant
	char buffer[1024];

	// Essayer de recevoir des données initiales sans bloquer
	ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	buffer[bytes_received] = '\0';
	std::string answer(buffer);

	if (findCapLs(answer) == 0) {
		std::cerr << "connected with irssi!\n\n";
		this->_irssi_data = answer;
		parsingDataIrssi(_clients[client_socket]);
		isRegistered(_clients[client_socket]); 
	}
	else {
		std::cerr << "connected with netcat!\n\n";
		std::cout << BLUE << "\n. . . Waiting for client registration . . . " << RESET << std::endl;
		parsingDataNetclient(_clients[client_socket], client_socket);
		isRegistered(_clients[client_socket]); 
	}
}
