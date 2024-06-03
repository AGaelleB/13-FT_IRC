#include "../includes/Server.hpp"

struct pollfd fds[1024];
int nfds = 1;

/************************************* CONST ET DEST *************************************/

Server::Server() : _server_socket(-1), _password("1234"), _port(6667) {
}

Server::Server(int port, const std::string &password) : _server_socket(-1), _password(password), _port(port) {
	// 1. Crée un socket: socket()
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1) {
		std::cerr << "Error: socket creation failed" << std::endl;
		exit(1);
	}

	// 2. Lie le socket à une adresse IP et un _port: bind(), htons()
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_addr.sin_port = htons(_port);

	if (bind(_server_socket, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) {
		std::cerr << "Error: bind failed" << std::endl;
		close(_server_socket);
		exit(1);
	}

	// 3. Met le socket en écoute pour les connexions entrantes: listen()
	if (listen(_server_socket, SOMAXCONN) == -1) {
		std::cerr << "Error: listen failed" << std::endl;
		close(_server_socket);
		exit(1);
	}

	// Initialiser la structure pollfd pour le serveur
		fds[0].fd = _server_socket;
		fds[0].events = POLLIN;  // Surveiller les événements d'entrée
}

Server::~Server() {
	if (_server_socket != -1)
		close(_server_socket);
}


/************************************** FUNCTIONS **************************************/

void Server::addUser(Client &client, const std::string &username, const std::string &nickname) {
	static int current_index = 1;
	User user(current_index++, username, nickname);
	client.setUser(user);
	_clients[client.getClientSocket()] = client;
}

std::string trim(const std::string& str) {
	size_t start = 0;
	while (start < str.size() && std::isspace(str[start])) {
		start++;
	}

	size_t end = str.size();
	while (end > start && std::isspace(str[end - 1])) {
		end--;
	}

	return (str.substr(start, end - start));
}

void Server::startServer() {
    std::cout << bannerServer;
    std::cout << ". . . Listening on _port " << _port << " . . . " << std::endl;

    while (true) {
        int poll_count = poll(fds, nfds, -1);
        if (poll_count == -1) {
            std::cerr << "Error: poll failed" << std::endl;
            exit(1);
        }

        for (int i = 0; i < nfds; ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == _server_socket) {
                    // Nouvelle connexion
                    Client client; // Initialiser un nouvel objet client
                    socklen_t client_len = sizeof(client.getClientAddr());
                    int client_socket = accept(_server_socket, (struct sockaddr*)&client.getClientAddr(), &client_len);
                    if (client_socket == -1) {
                        std::cerr << "Error: connection not accepted" << std::endl;
                        continue;
                    }

                    // Ajouter le nouveau client au vecteur pollfd
                    fds[nfds].fd = client_socket;
                    fds[nfds].events = POLLIN;
                    nfds++;

                    client.setClientSocket(client_socket);
                    std::cout << "\nNew connection accepted ​✅" << std::endl;

                    client.sendClientMsg(client_socket, bannerIRC);
                    client.welcomeClient(client_socket);

                    // Gérer l'authentification et les informations utilisateur
                    authenticateAndRegister(client);

                    // Ajouter le client au map des clients après l'authentification
                    _clients[client_socket] = client;
                } else {
                    // Message reçu d'un client existant
                    handleClientMessage(i);
                }
            }
        }
    }
}

void Server::authenticateAndRegister(Client &client) {
    char buffer[1024];
    ssize_t bytes_received;

    while (true) {
        const char* pass_msg = BOLD "Enter Server password: " RESET;
        client.sendClientMsg(client.getClientSocket(), pass_msg);

        bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cerr << "Error: reception failed" << std::endl;
            close(client.getClientSocket());
            return;
        }
        buffer[bytes_received] = '\0';
        std::string pass(buffer);
        pass = trim(pass);

        if (pass != this->_password) {
            const char* invalid_pass = RED "Wrong password \n\n" RESET;
            client.sendClientMsg(client.getClientSocket(), invalid_pass);
        } else {
            break;
        }
    }

    client.sendClientMsg(client.getClientSocket(), "Enter your username: ");
    bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        std::cerr << "Error: reception failed" << std::endl;
        close(client.getClientSocket());
        return;
    }
    buffer[bytes_received] = '\0';
    std::string username = trim(std::string(buffer));

    client.sendClientMsg(client.getClientSocket(), "Enter your nickname: ");
    bytes_received = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        std::cerr << "Error: reception failed" << std::endl;
        close(client.getClientSocket());
        return;
    }
    buffer[bytes_received] = '\0';
    std::string nickname = trim(std::string(buffer));

    addUser(client, username, nickname);
    client.sendClientMsg(client.getClientSocket(), "You are now registered!\n");
}

void Server::handleClientMessage(int client_index) {
    char buffer[1024];
    ssize_t bytes_received = recv(fds[client_index].fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            std::cout << "Client disconnected ❌" << std::endl;
        } else {
            std::cerr << "Error: data reception failed" << std::endl;
        }
        close(fds[client_index].fd);
        _clients.erase(fds[client_index].fd); // Supprimer le client de la map
        fds[client_index] = fds[nfds - 1];  // Remplacer par le dernier
        nfds--;
    } else {
        buffer[bytes_received] = '\0';
        std::string message(buffer);
        _clients[fds[client_index].fd].handleClientMsg(message, _clients[fds[client_index].fd]);
    }
}
