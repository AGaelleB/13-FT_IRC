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

void Server::SignalHandler(int sig) {
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::_shutdown_signal = true;
	(void)sig;
}


void Server::TstpSignalHandler(int sig) {
	std::cout << std::endl << "SIGTSTP (Ctrl+Z) Received! Ignoring suspension" << std::endl;
	(void)sig;
}