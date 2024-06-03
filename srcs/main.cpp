#include "../includes/Server.hpp"

int main(int ac, char **av) {
	if (ac != 3) {
		std::cerr << RED << "Error: must be ./Server <port> <password>" << RESET << std::endl;
		return (-1);
	}

	int port = std::atoi(av[1]);
	if (port < 1000 || port > 9999 || std::strlen(av[1]) != 4) {
		std::cout << RED << "Error: port must contain 4 digits" << RESET << std::endl;
		return (-1);
	}

	std::string password = av[2];
	if (password.length() >= 10) {
		std::cerr << RED << "Error: password must be less than 10 characters" << RESET << std::endl;
		return (-1);
	}

	Server server(port, password);
	server.startServer();

	return (0);
}

/* 
Serveur:
1. Crée un socket: socket()
2. Lie le socket à une adresse IP et un port: bind(), htons()
3. Met le socket en écoute pour les connexions entrantes: listen()
4. Accepte les connexions des clients: accept()
5. Échange des données avec les clients: send(), recv()
6. Ferme le socket: close()

Client:
1. Crée un socket: socket()
2. Se connecte à l'adresse IP et au port du serveur: connect(), inet_addr(), htons()
3. Échange des données avec le serveur: send(), recv()
4. Ferme le socket: close()

 */
