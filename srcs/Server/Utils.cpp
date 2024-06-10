#include "../../includes/Server.hpp"

std::string trim(const std::string& str) {
	size_t start = 0;
	while (start < str.size() && (std::isspace(str[start]) || str[start] == '\r')) {
		start++;
	}
	size_t end = str.size();
	while (end > start && (std::isspace(str[end - 1]) || str[end - 1] == '\r')) {
		end--;
	}
	return str.substr(start, end - start);
}

// std::vector<std::string> split(const std::string &str) {
// 	std::vector<std::string> tokens;
// 	size_t pos = str.find_first_of(" \n");

// 	if (pos != std::string::npos) {
// 		tokens.push_back(str.substr(0, pos));
// 		if (pos + 1 < str.length())
// 			tokens.push_back(str.substr(pos + 1));
// 	}
// 	else
// 		tokens.push_back(str);
// 	return (tokens);
// }

std::vector<std::string> split(const std::string &str) {
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;

	while (iss >> token)
		tokens.push_back(token);
	return (tokens);
}

void Server::SignalHandler(int sig) {
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::shutdown_signal = true; // to stop the server
	(void)sig;
}
