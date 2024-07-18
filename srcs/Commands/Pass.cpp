#include "../../includes/Server.hpp"

bool Server::checkPasswordirssi(std::string answer, Client& client) {
	std::string password;
	std::istringstream stream(answer);
	std::string line;
	while (std::getline(stream, line)) {
		size_t pos = line.find("PASS");
		if (pos != std::string::npos) {
			password = line.substr(pos + 5);
			password = trim(password);
			if (password != this->_password) {
				client.sendClientMsg(client.getClientSocket(), ERROR_PASSWORD);
				return (0);
			}
			return (1);
		}
	}
	return (0);
}