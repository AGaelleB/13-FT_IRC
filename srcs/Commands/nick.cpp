#include "../../includes/Server.hpp"

bool Server::isNicknameAvailable(const std::string& nickname) {
	return (_nicknames.find(nickname) == _nicknames.end());
}

void Server::removeNickname(const std::string& nickname) {
	std::cout << MAGENTA << "remove 1 old nickname : " << nickname << RESET << std::endl;
	_nicknames.erase(nickname);
	std::cout << MAGENTA << "remove 2 old nickname : " << nickname << RESET << std::endl;
	
	if (_nicknames.find(nickname) == _nicknames.end()) {
		std::cout << MAGENTA << "Nickname " << nickname << " successfully removed." << RESET << std::endl;
	} else {
		std::cout << MAGENTA << "Failed to remove nickname: " << nickname << RESET << std::endl;
	}
}

void Server::addNickname(const std::string& nickname) {
	_nicknames.insert(nickname);
}

void	Server::nickCmdClient(std::vector<std::string> tokens, Client& client) {
	std::string new_nickname = tokens[1];
	std::cout << new_nickname << std::endl;
	
	new_nickname.erase(std::remove(new_nickname.begin(), new_nickname.end(), '\n'), new_nickname.end());
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
		removeNickname(old_nickname);
		std::cout << MAGENTA << "cmd old nickname : " << old_nickname << RESET << std::endl;
		// ici faire une suppression de l'ancien nickname pour le rendre accessible une fois que le client a changer
		client.getUser().setNickname(new_nickname);
		addNickname(new_nickname);
	}
	else
		client.sendClientMsg(client.getClientSocket(), ERROR_NICKNAME_NOT_AVAILABLE);
}
