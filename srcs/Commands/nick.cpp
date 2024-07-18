#include "../../includes/Server.hpp"

/* 
	/nick <new_nickname>
 */

bool Server::isNicknameAvailable(const std::string& nickname) {
	return (_nicknames.find(nickname) == _nicknames.end());
}

void Server::removeNickname(const std::string& nickname) {
	_nicknames.erase(nickname);

	if (_nicknames.find(nickname) == _nicknames.end())
		std::cout << GREEN << "The nickname: " << nickname << " has been weel removed" << RESET << std::endl;
	else
		std::cout << RED << "Failed to remove nickname: " << nickname << RESET << std::endl;
}

void Server::addNickname(const std::string& nickname) {
	_nicknames.insert(nickname);
}

void Server::nickCmdClient(std::vector<std::string> tokens, Client& client) {
	if (tokens.size() < 2) {
		std::string errorNetcat = std::string(RED) + "Error: must be: /nick <new_nickname>\n" + std::string(RESET);
		std::string errorIrssi = ":localhost 431 " + client.getUser().getNickname() + " :No nickname given\r\n";
		sendErrorMessage(client, errorNetcat, errorIrssi);
		return;
	}

	std::string new_nickname = trim(tokens[1]);
	if (new_nickname.empty()) {
		std::string errorNetcat = std::string(RED) + "Error: must be: /nick <new_nickname>\n" + std::string(RESET);
		std::string errorIrssi = ":localhost 431 " + client.getUser().getNickname() + " :No nickname given\r\n";
		sendErrorMessage(client, errorNetcat, errorIrssi);
		return;
	}

	if (!client.checkName(new_nickname)) {
		std::string errorNetcat = std::string(RED) + "Error: Invalid nickname\n" + std::string(RESET);
		std::string errorIrssi = ":localhost 432 " + client.getUser().getNickname() + " " + new_nickname + " :Erroneous nickname\r\n";
		sendErrorMessage(client, errorNetcat, errorIrssi);
		return;
	}

	if (isNicknameAvailable(new_nickname)) {
		std::string old_nickname = client.getUser().getNickname();
		if (!old_nickname.empty())
			removeNickname(old_nickname);

		client.getUser().setNickname(new_nickname);
		addNickname(new_nickname);

		std::cout << MAGENTA << "Changed nickname from " << old_nickname << " to " << new_nickname << RESET << std::endl;
		
		std::string successNetcat = std::string(GREEN) + "Nickname changed to " + new_nickname + "\n" + std::string(RESET);
		std::string successIrssi = ":" + old_nickname + "!" + client.getUser().getUsername() + "@localhost NICK " + new_nickname + "\r\n";
		sendErrorMessage(client, successNetcat, successIrssi);
	}
	else {
		std::string errorNetcat = std::string(RED) + "Error: Nickname is already in use\n" + std::string(RESET);
		std::string errorIrssi = ":localhost 433 " + client.getUser().getNickname() + " " + new_nickname + " :Nickname is already in use\r\n";
		sendErrorMessage(client, errorNetcat, errorIrssi);
	}
}

// /connect localhost 6667 1
