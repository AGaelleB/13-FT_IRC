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
