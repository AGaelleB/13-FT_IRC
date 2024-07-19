#include "../../includes/Server.hpp"

/************************************** FUNCTIONS **************************************/


void Channel::addBannedWord(const std::string& word) {
	_bannedWords.insert(word);
}

void Channel::removeBannedWord(const std::string& word) {
	_bannedWords.erase(word);
}

bool Channel::isBannedWord(const std::string& word) const {
	return (_bannedWords.find(word) != _bannedWords.end());
}

void Server::addBannedWordCmd(Client& client, const std::vector<std::string>& tokens, Channel& channel) {
	if (tokens.size() < 4) {
		std::string errorMessage = "Error: Must be: /MODE <channel> <+b> <word>\r\n";
		std::string irssiMessage = ":localhost 461 " + client.getUser().getNickname() + " MODE_B :Not enough parameters\r\n";
		sendErrorMessage(client, errorMessage, irssiMessage);
		return;
	}

	if (!channel.isOperator(client.getClientSocket())) {
		std::string errorMessage = "Error: Only channel operators can add banned words.\r\n";
		std::string irssiMessage = ":localhost 482 " + client.getUser().getNickname() + " " + channel.getName() + " :You're not channel operator\r\n";
		sendErrorMessage(client, errorMessage, irssiMessage);
		return;
	}

	std::string bannedWord = tokens[3];
	channel.addBannedWord(bannedWord);

	std::string successMessage = "The word \"" + bannedWord + "\" has been added to the banned list.\r\n";
	send(client.getClientSocket(), successMessage.c_str(), successMessage.size(), 0);
}
