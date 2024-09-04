void Server::detectClient(int client_socket) {
    char buffer[1024] = {0};
    bool data_received = false;
	const int max_attempts = 10;
	const int delay_between_attempts = 5000;


    // Vérifiez si poll_count indique que des événements ont été détectés
	for (int attempt = 0; attempt < max_attempts; ++attempt) {
		usleep(delay_between_attempts);
		ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
		if (bytes_received > 0) {
			buffer[bytes_received] = '\0';
			data_received = true;
			break;
		}
		if (poll_count > 0) {
			// Recherchez dans fds si le client_socket est prêt
			for (int i = 0; i < nfds; ++i) {
				if (fds[i].fd == client_socket && (fds[i].revents & POLLIN)) {
					ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
					if (bytes_received > 0) {
						buffer[bytes_received] = '\0';
						data_received = true;
					}
					else if (bytes_received == 0) {
						std::cerr << RED << "DETECT Client disconnected [socket: " << client_socket << "]" << RESET << std::endl;
						close(client_socket);
						_clients.erase(client_socket);
						return;
					}
					else {
						std::cerr << RED << "Error receiving data [socket: " << client_socket << "]" << RESET << std::endl;
						close(client_socket);
						_clients.erase(client_socket);
						return;
					}
				}
			}
		}
		else {
			std::cerr << BLUE << "No data received within the timeout period. Continuing..." << RESET << std::endl;
		}
	}

    // Accéder à l'objet client par référence
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
                std::cerr << RED << "\nClient disconnected during registration process, client_socket: " << client.getClientSocket() << RESET << std::endl;
                close(client_socket);
                _clients.erase(client_socket);
            }
        }
		else {
            std::cerr << RED << "\nClient disconnected during registration process, client_socket: " << client.getClientSocket() << RESET << std::endl;
            close(client_socket);
            _clients.erase(client_socket);
        }
    }
}
