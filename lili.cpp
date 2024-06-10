
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
		// ici faire une suppression de l'ancien nickname pour le rendre accessible une fois que le client a changer
		client.getUser().setNickname(new_nickname);
		addNickname(new_nickname);
		std::cout << MAGENTA << "NEW Nickname " << client.getUser().getNickname() << RESET << std::endl;
	}
	else
		client.sendClientMsg(client.getClientSocket(), ERROR_NICKNAME_NOT_AVAILABLE);
}
