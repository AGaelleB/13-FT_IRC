#include "../../includes/Server.hpp"

std::string trim(const std::string& str) {
	size_t start = 0;
	while (start < str.size() && (std::isspace(str[start]) || str[start] == '\r'))
		start++;
	size_t end = str.size();
	while (end > start && (std::isspace(str[end - 1]) || str[end - 1] == '\r'))
		end--;
	return str.substr(start, end - start);
}

std::string trimStrings(const std::string& str) {
	size_t first = str.find_first_not_of(' ');
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, last - first + 1);
}

std::vector<std::string> split(const std::string &str) {
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;

	while (iss >> token)
		tokens.push_back(token);
	return (tokens);
}

std::vector<std::string> splitComa(const std::string& str) {
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;
	
	while (std::getline(iss, token, ','))
		tokens.push_back(trimStrings(token));

	return (tokens);
}

void cleanUp() {
    std::cout << "Cleaning up resources..." << std::endl;

    // Itérateur pour parcourir la map des clients
    std::map<int, Client>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it) {
        close(it->first); // Ferme le socket du client
    }
    _clients.clear(); // Vide la map des clients

    close(_server_socket); // Ferme le socket du serveur

    // Réinitialiser les structures pollfd
    nfds = 0;

    std::cout << "All sockets closed." << std::endl;
}



// Déclaration de la fonction cleanUp globale
extern void cleanUp();

void Server::SignalHandler(int sig) {
    std::cout << std::endl << "Signal Received! Cleaning up and shutting down..." << std::endl;
    Server::_shutdown_signal = true;
    cleanUp(); // Appelez la fonction de nettoyage globale
    (void)sig;
}

void Server::TstpSignalHandler(int sig) {
    std::cout << std::endl << "SIGTSTP (Ctrl+Z) Received! Ignoring suspension" << std::endl;
    (void)sig;
}