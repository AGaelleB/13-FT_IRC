#include "../../../includes/Server.hpp"

void Server::modeBCmd(Client& client, const std::vector<std::string>& tokens, Channel& channel, const std::string& channelName) {
	if (tokens.size() < 4) {
		std::string errorMessage = "Error: Must be: /MODE <channel> <+b/-b> <word>\r\n";
		std::string irssiMessage = ":localhost 461 " + client.getUser().getNickname() + " MODE_B :Not enough parameters\r\n";
		sendErrorMessage(client, errorMessage, irssiMessage);
		return;
	}

	if (!channel.isOperator(client.getClientSocket())) {
		std::string errorMessage = "Error: Only channel operators can modify banned words.\r\n";
		std::string irssiMessage = ":localhost 482 " + client.getUser().getNickname() + " " + channel.getName() + " :You're not channel operator\r\n";
		sendErrorMessage(client, errorMessage, irssiMessage);
		return;
	}

	std::string mode = tokens[2];
	std::string bannedWord = tokens[3];

	if (mode == "+b") {
		channel.addBannedWord(bannedWord);
		std::string successMessage = "The word \"" + bannedWord + "\" has been added to the banned list in " + channelName + ".\r\n";
		send(client.getClientSocket(), successMessage.c_str(), successMessage.size(), 0);
	}
	else if (mode == "-b") {
		channel.removeBannedWord(bannedWord);
		std::string successMessage = "The word \"" + bannedWord + "\" has been removed from the banned list in " + channelName + ".\r\n";
		send(client.getClientSocket(), successMessage.c_str(), successMessage.size(), 0);
	}
	else {
		std::string errorMessage = "Error: Unknown mode command.\r\n";
		std::string irssiMessage = ":localhost 472 " + client.getUser().getNickname() + " " + mode + " :is unknown mode char to me\r\n";
		sendErrorMessage(client, errorMessage, irssiMessage);
	}
}
