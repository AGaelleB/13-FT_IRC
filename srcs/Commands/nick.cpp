#include "../../includes/Server.hpp"

bool Server::isNicknameAvailable(const std::string& nickname) {
	return (_nicknames.find(nickname) == _nicknames.end());
}

void Server::removeNickname(const std::string& nickname) {
	_nicknames.erase(nickname);
	
	if (_nicknames.find(nickname) == _nicknames.end()) {
		// std::cout << MAGENTA << "OLD Nickname " << nickname  << RESET << std::endl;
	}
	else
		std::cout << MAGENTA << "Failed to remove nickname: " << nickname << RESET << std::endl;
}

void Server::addNickname(const std::string& nickname) {
	_nicknames.insert(nickname);

}

void Server::nickCmdClient(std::vector<std::string> tokens, Client& client) {

	if (tokens.size() < 2) {
		client.sendClientMsg(client.getClientSocket(), ERROR_NEW_NICKNAME);
		return;
	}

	std::string new_nickname = trim(tokens[1]);
	if (new_nickname.empty()) {
		client.sendClientMsg(client.getClientSocket(), ERROR_NEW_NICKNAME);
		return;
	}

	if (!client.checkName(new_nickname)) {
		client.sendClientMsg(client.getClientSocket(), ERROR_NICKNAME);
		return;
	}

	if (isNicknameAvailable(new_nickname)) {
		std::string old_nickname = client.getUser().getNickname();
		if (!old_nickname.empty())
			removeNickname(old_nickname);

		client.getUser().setNickname(new_nickname);
		addNickname(new_nickname);

		std::cout << MAGENTA << "Changed nickname from " << old_nickname << " to " << new_nickname << RESET << std::endl;
	}
	else
		client.sendClientMsg(client.getClientSocket(), ERROR_NICKNAME_NOT_AVAILABLE);
}