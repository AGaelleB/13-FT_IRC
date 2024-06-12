#include "../../includes/Server.hpp"

// Définition de la variable statique _shutdown_signal
bool Server::_shutdown_signal = false;

struct pollfd fds[200];
int nfds = 1;

/************************************* CONST ET DEST *************************************/
Server::Server(int port, const std::string &password) : _server_socket(-1), _password(password), _port(port) {
    // 1. Crée un socket: socket()
    _server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket == -1) {
        throw std::runtime_error("Socket creation error: " + std::string(strerror(errno)));
    }

    // Configurer l'option SO_REUSEADDR pour pas bloquer le port
    int opt = 1;
    if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        close(_server_socket);
        throw std::runtime_error("Set socket option error: " + std::string(strerror(errno)));
    }

    // 2. Lie le socket à une adresse IP et un _port: bind(), htons()
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = INADDR_ANY;
    _server_addr.sin_port = htons(_port);

    if (bind(_server_socket, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) {
        close(_server_socket);
        throw std::runtime_error("Socket bind error: " + std::string(strerror(errno)));
    }

    // 3. Met le socket en écoute pour les connexions entrantes: listen()
    if (listen(_server_socket, SOMAXCONN) == -1) {
        close(_server_socket);
        throw std::runtime_error("Socket listen error: " + std::string(strerror(errno)));
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
void Server::sendDisconnectMessageToClients() {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        send(it->first, MSG_DISCONNECT, strlen(MSG_DISCONNECT), 0);
    }
}

void Server::startServer() {
    std::cout << bannerServer;
    std::cout << BLUE << ". . . Listening on port " << _port << " . . . " << RESET << std::endl;

    signal(SIGINT, Server::SignalHandler); // catch the signal (ctrl + c)
    signal(SIGQUIT, Server::SignalHandler); // catch the signal (ctrl + \)

    while (!_shutdown_signal) {
        int poll_count = poll(fds, nfds, 1000);
        if (poll_count == -1) {
            if (errno == EINTR)
                continue; // Interrompu par un signal
            std::cerr << "Error: poll failed" << std::endl;
            break;
        }
        for (int i = 0; i < nfds; ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == _server_socket) {
                    // Nouvelle connexion
                    Client client;
                    socklen_t client_len = sizeof(client.getClientAddr());
                    int new_client_socket = accept(_server_socket, (struct sockaddr*)&client.getClientAddr(), &client_len);
                    if (new_client_socket == -1) {
                        std::cerr << "Error: connection not accepted: " << std::string(strerror(errno)) << std::endl;
                        continue;
                    }
                    std::cout << GREEN << "\nNew connection accepted! ✅ ---> client_socket: " << new_client_socket << RESET << std::endl;
                    std::cout << BOLD << "Total client(s) online: " << RESET << nfds << std::endl;
                    fds[nfds].fd = new_client_socket;
                    fds[nfds].events = POLLIN;
                    nfds++;
                    _clients[new_client_socket] = client;
                    detectClient(new_client_socket);
                } else {
                    handleClientMessage(fds[i].fd, _clients[fds[i].fd]);
                }
            }
        }
    }
    stopServer();
}

void Server::stopServer() {
    std::cout << "Shutting down server..." << std::endl;
    sendDisconnectMessageToClients();
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        close(it->first);
    close(_server_socket);
    std::cout << "Server stopped." << std::endl;
}