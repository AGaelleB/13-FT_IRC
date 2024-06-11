#include "../../includes/Server.hpp"

bool Server::checkPasswordirssi(std::string answer, Client& client) {
    std::string password;
    // std::cout << ORANGE << "checkPasswordirssi" << RESET << std::endl;

    std::istringstream stream(answer);
    std::string line;
    while (std::getline(stream, line)) {
        size_t pos = line.find("PASS");
        if (pos != std::string::npos) {
            password = line.substr(pos + 5);
            password = trim(password);
            // std::cout << ORANGE << "INSIDE password = [" << password << "]" << RESET << std::endl;

            if (password != this->_password) {
                // std::cout << YELLOW << password << RESET << std::endl; ///
                client.sendClientMsg(client.getClientSocket(), ERROR_PASSWORD);
                return (0);
            }
            return (1);
        }
    }
    return (0);
}