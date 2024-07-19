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

bool Server::checkBannedWordsPrivMsg(Client& client, Channel& channel, const std::string& channelName, const std::string& msgContent) {
	std::istringstream iss(msgContent);
	std::string word;
	while (iss >> word) {
		std::cout << YELLOW << word << RESET << std::endl;
		if (channel.isBannedWord(word)) {
			// Expulser le client si un mot interdit est trouvé
			channel.removeMember(client.getClientSocket());

			std::string kickMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + std::string(CYAN_IRSSI) + client.getUser().getNickname() + std::string(RESET) + " [" + std::string(CYAN_IRSSI) + client.getUser().getUsername() + "@localhost" + std::string(RESET) + "] has been kicked from " + std::string(BOLD) + channelName + std::string(RESET) + " for using the banned word.\r\n";
			std::string kickMsgIrssi = RPL_KICK(client.getUser().getNickname(), client.getUser().getUsername(), channelName, client.getUser().getNickname(), "Has been kicked for using the banned wor.");

			// Diffuser le message de départ à tous les membres du canal
			const std::vector<int>& members = channel.getMembers();
			for (size_t i = 0; i < members.size(); ++i) {
				int memberSocket = members[i];
				Client& memberClient = _clients[memberSocket];

				std::string fullMessage;
				if (memberClient.isIrssi)
					fullMessage = kickMsgIrssi;
				else
					fullMessage = kickMsgNetcat;
				::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
			}

			std::string netcatKickMessage = std::string(RED) + "You have been kicked from the channel " + channelName + " for using the banned word \"" + word + "\".\n" + RESET;
			std::string irssiKickMessage = RPL_KICK(client.getUser().getNickname(), client.getUser().getUsername(), channelName, client.getUser().getNickname(), "You have been kicked for using the banned word \"" + word + "\".");
			std::string partMessage = RPL_PART(client.getUser().getNickname(), client.getUser().getUsername(), channelName, "You have been kicked from the channel.");

			if (client.isIrssi) {
				std::cout << YELLOW << "BANNED" << RESET << std::endl;
				client.sendClientMsg(client.getClientSocket(), partMessage.c_str());
				client.sendClientMsg(client.getClientSocket(), irssiKickMessage.c_str());
			} else {
				client.sendClientMsg(client.getClientSocket(), netcatKickMessage.c_str());
			}

			// Vérifier si le canal est vide après l'expulsion et le supprimer
			if (channel.getMembers().empty()) {
				_channels.erase(channelName);
				_channelOrder.erase(std::remove(_channelOrder.begin(), _channelOrder.end(), channelName), _channelOrder.end());
				std::cout << BOLD << "Channel: [" << channelName << "] destroyed successfully! ❌" << RESET << std::endl;
			}

			return (true);
		}
	}
	return (false);
}

bool Server::checkBannedWordsUnknown(Client& client, Channel& channel, const std::string& channelName, const std::string& msgContent) {
    std::istringstream iss(msgContent);
    std::string word;
    while (iss >> word) {
        std::cout << YELLOW << word << RESET << std::endl;
        if (channel.isBannedWord(word)) {
            // Expulser le client si un mot interdit est trouvé
            channel.removeMember(client.getClientSocket());

            std::string kickMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + std::string(CYAN_IRSSI) + client.getUser().getNickname() + std::string(RESET) + " [" + std::string(CYAN_IRSSI) + client.getUser().getUsername() + "@localhost" + std::string(RESET) + "] has been kicked from " + std::string(BOLD) + channelName + std::string(RESET) + " for using the banned word.\r\n";
            std::string kickMsgIrssi = RPL_KICK(client.getUser().getNickname(), client.getUser().getUsername(), channelName, client.getUser().getNickname(), "Has been kicked for using the banned word.");

            // Diffuser le message de départ à tous les membres du canal
            const std::vector<int>& members = channel.getMembers();
            for (size_t i = 0; i < members.size(); ++i) {
                int memberSocket = members[i];
                Client& memberClient = _clients[memberSocket];

                std::string fullMessage;
                if (memberClient.isIrssi)
                    fullMessage = kickMsgIrssi;
                else
                    fullMessage = kickMsgNetcat;
                ::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
            }

            std::string netcatKickMessage = std::string(RED) + "You have been kicked from the channel " + channelName + " for using the banned word \"" + word + "\".\n" + RESET;
            std::string irssiKickMessage = RPL_KICK(client.getUser().getNickname(), client.getUser().getUsername(), channelName, client.getUser().getNickname(), "You have been kicked for using the banned word \"" + word + "\".");
            std::string partMessage = RPL_PART(client.getUser().getNickname(), client.getUser().getUsername(), channelName, "You have been kicked from the channel.");

            if (client.isIrssi) {
                std::cout << YELLOW << "BANNED" << RESET << std::endl;
                client.sendClientMsg(client.getClientSocket(), partMessage.c_str());
                client.sendClientMsg(client.getClientSocket(), irssiKickMessage.c_str());
            }
			else {
                client.sendClientMsg(client.getClientSocket(), netcatKickMessage.c_str());
            }

            // Vérifier si le canal est vide après l'expulsion et le supprimer
            if (channel.getMembers().empty()) {
                _channels.erase(channelName);
                _channelOrder.erase(std::remove(_channelOrder.begin(), _channelOrder.end(), channelName), _channelOrder.end());
                std::cout << BOLD << "Channel: [" << channelName << "] destroyed successfully! ❌" << RESET << std::endl;
            }

            return (true);
        }
    }
	return (false);
}
