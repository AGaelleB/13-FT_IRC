void Server::detectClient(int client_socket) {
	Client& client = _clients[client_socket];
	client.sendClientMsg(client_socket, bannerIRC);
	client.sendClientMsg(client_socket, MSG_WELCOME);
	client.sendClientMsg(client_socket, MSG_HELP_CLIENT);
	authenticateAndRegister(client);
	_clients[client_socket] = client; // Ajout du client à la map
}

void Server::authenticateAndRegister(Client& client) {
	client.sendClientMsg(client.getClientSocket(), "Enter Server password: ");
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	ssize_t bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
	if (bytes_received <= 0) {
		std::cerr << "Error: failed to receive password" << std::endl;
		return;
	}
	buffer[bytes_received] = '\0';
	std::string password(buffer);

	if (password == _password) {
		client.sendClientMsg(client.getClientSocket(), "Enter your username: ");
		bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
		if (bytes_received <= 0) {
			std::cerr << "Error: failed to receive username" << std::endl;
			return;
		}
		buffer[bytes_received] = '\0';
		std::string username(buffer);

		client.sendClientMsg(client.getClientSocket(), "Enter your nickname: ");
		bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
		if (bytes_received <= 0) {
			std::cerr << "Error: failed to receive nickname" << std::endl;
			return;
		}
		buffer[bytes_received] = '\0';
		std::string nickname(buffer);

		addUser(client, username, nickname);
		client.sendClientMsg(client.getClientSocket(), "You are now registered! ✅\n");
	} else {
		client.sendClientMsg(client.getClientSocket(), "Incorrect password. Connection closed.\n");
		close(client.getClientSocket());
		_clients.erase(client.getClientSocket());
	}
}