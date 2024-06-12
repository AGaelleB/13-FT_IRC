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

    try {
        Server server(port, password);
        server.startServer();
    } catch (const std::exception &e) {
        std::cerr << RED << "ERROR: " << e.what() << RESET << std::endl;
        return (-1);
    }

    return (0);
}

/* TO DO

ATTENTION AU PARSING DE NICKNAME, lorsque l on se connecte avec irssi, 
on parse directement tout le message CAP LS etc, et du coup on essaie d affecter directement le NICKNAME

Voir pour envoyer le msg Error: must be ./Server <port> <password> sur irsssi

/connect localhost 6667 1

 */