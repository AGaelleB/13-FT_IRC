#include "../../includes/Server.hpp"

void Server::broadcastMessageToChannel(const std::string& channelName, const std::string& message, int excludeSocket) {
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it != _channels.end()) {
		Channel& channel = it->second;

		// Vérifier si le message contient des mots interdits
		std::istringstream iss(message);
		std::string word;
		while (iss >> word) {
			if (channel.isBannedWord(word)) {
				// Expulser le client si un mot interdit est trouvé
				channel.removeMember(excludeSocket);
				std::string irssiKickMessage = ":localhost 474 " + _clients[excludeSocket].getUser().getNickname() + " " + channelName + " :You have been kicked for using the banned word \"" + word + "\".\r\n";
				std::string netcatKickMessage = std::string(RED) + "You have been kicked from the channel " + channelName + " for using the banned word \"" + word + "\".\n" + RESET;
				sendErrorMessage(_clients[excludeSocket], netcatKickMessage, irssiKickMessage);
				return;
			}
		}

		const std::vector<int>& members = channel.getMembers();
		for (std::vector<int>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
			if (*memberIt != excludeSocket) {
				std::map<int, Client>::iterator clientIt = _clients.find(*memberIt);
				if (clientIt != _clients.end()) {
					std::string fullMessage;
					if (clientIt->second.isIrssi)
						fullMessage = ":" + _clients[excludeSocket].getUser().getNickname() + " PRIVMSG " + channelName + " :" + message + "\r\n";
					else
						fullMessage = "[" + channelName + "] <" + _clients[excludeSocket].getUser().getNickname() + "> " + message + "\r\n";

					::send(clientIt->second.getClientSocket(), fullMessage.c_str(), fullMessage.size(), 0);
				}
			}
		}
	}
}

void Server::ChannelMsg(Client& client) {

	char buffer[MAX_SIZE_MSG];
	int bytes_read = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
	if (bytes_read <= 0) {
		// Handle client disconnection
		close(client.getClientSocket());
		fds[client.getClientSocket()].fd = -1;
		_clients.erase(client.getClientSocket());
	}
	else {
		buffer[bytes_read] = '\0';
		std::string message(buffer);

		// Check if client is part of a channel and broadcast the message
		for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
			Channel& channel = it->second;
			if (channel.isMember(client.getClientSocket())) {
				std::string fullMessage = "<" + client.getUser().getNickname() + "> " + message;
				broadcastMessageToChannel(it->first, fullMessage, client.getClientSocket());
				break;
			}
		}
	}
}
