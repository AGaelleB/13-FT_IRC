#include "../includes/main.hpp"

int main(int ac, char **av) {

	Server *server;

	if (ac != 3) {
		std::cerr << RED << "Error: must be ./Server <port> <password>" << RESET << std::endl;
		return 1;
	}
	(void)av;

	// 1. Crée un socket: socket()
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		std::cerr << "Erreur lors de la création du socket" << std::endl;
		return 1;
	}
	std::cout << "Socket créé avec succès, descripteur : " << server_socket << std::endl;

	// 2. Lie le socket à une adresse IP et un port: bind(), htons()
	server->server_addr.sin_family = AF_INET;
	server->server_addr.sin_addr.s_addr = 
	server->server_addr.sin_port = 



	close(server_socket); // Fermer le socket
	return 0;
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