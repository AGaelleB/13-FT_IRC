#include "../../includes/Server.hpp"

/************************************** FUNCTIONS **************************************/

bool Server::validateTokensJoin(Client& client, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2 && tokens.size() > 3) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_CHANNEL);
		return (false);
	}
	return (true);
}


void Server::handleChannel(Client& client, std::string& channelName, const std::vector<std::string>& tokens) {
	if (channelName[0] != '#') {
		channelName = "#" + channelName;
	}

	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end()) {
		createChannel(client, channelName);
		usleep(42); // Assurez-vous que cette pause est nécessaire

		it = _channels.find(channelName);
		if (it == _channels.end()) {
			client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_FAILED_CREATE);
			return;
		}
	}

	Channel& channel = it->second;

	// Vérifier si le client est déjà membre du canal
	if (channel.isMember(client.getClientSocket())) {
		std::stringstream ss;
		ss << MSG_ALREADY_CHAN << channelName << std::endl << std::endl;
		std::string channelMsg = ss.str();
		client.sendClientMsg(client.getClientSocket(), channelMsg.c_str());
		return;
	}

	// Vérifier si le canal est protégé par un mot de passe
	if (!channel.getChannelKey().empty()) {
		if (tokens.size() < 3 || tokens[2] != channel.getChannelKey()) {
			client.sendClientMsg(client.getClientSocket(), ERROR_WRONG_CHANNEL_PASS);
			return;
		}
	}

	// Vérifier si le canal est plein
	if (channel.getMembersCount() >= channel.getMaxMembers()) {
		client.sendClientMsg(client.getClientSocket(), "ERROR_CHANNEL_FULL");
		return;
	}

	// Vérifier si le canal est en mode invitation seulement (+i)
	if (channel.getInvitationAccess() && !channel.isInvitedMember(client.getClientSocket())) {
		std::cout << YELLOW << "MODE +i true" << RESET << std::endl;
		if (client.isIrssi == true) {
			std::string inv_only = ERR_INVITEONLYCHAN(channelName);
			::send(client.getClientSocket(), inv_only.c_str(), inv_only.size(), 0);
		}
		else {
			std::string inv_only = "Cannot join this channel : you need an invite.\n";
			::send(client.getClientSocket(), inv_only.c_str(), inv_only.size(), 0);
		}
		return;
	}

	// Ajouter le client au canal
	channel.addMember(client.getClientSocket());
	sendChannelJoinInfo(channel, channelName, client);
}

void Server::sendChannelJoinInfo(Channel& channel, const std::string& channelName, Client& client) {
	std::string nick = client.getUser().getNickname();
	std::string username = client.getUser().getUsername();
	std::string listOfMembers = PrintChannelListMembers(channelName, _channels);
	std::string toSend;

	for (size_t i = 0; i < channel.getMembers().size(); ++i) {
		int memberSocket = channel.getMembers()[i];
		Client& member = _clients[memberSocket];
		toSend.clear();

 		if (member.isIrssi) {
			toSend = RPL_JOIN(user_id(nick, username), channelName);
			send(member.getClientSocket(), toSend.c_str(), toSend.size(), 0);
			toSend = RPL_PRIVMSG(nick, username, channel.getName(), listOfMembers);
			send(member.getClientSocket(), toSend.c_str(), toSend.size(), 0);
			
			if (channel.getTopic().getTopicName().empty())
				client.sendClientMsg(client.getClientSocket(), RPL_NOTOPIC(client.getUser().getNickname(), channelName).c_str());
			else 
				client.sendClientMsg(client.getClientSocket(), RPL_TOPIC(client.getUser().getNickname(), channelName,  channel.getTopic().getTopicName()).c_str());
		} 
		else {
			toSend = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + std::string(CYAN_IRSSI) + nick + std::string(RESET) + " [" + std::string(CYAN_IRSSI) + username + "@localhost" + std::string(RESET) + "]" + " has joined " + std::string(BOLD) + channelName + std::string(RESET) + "\r\n";
					
			send(member.getClientSocket(), toSend.c_str(), toSend.size(), 0);

			toSend = listOfMembers;
			send(member.getClientSocket(), toSend.c_str(), toSend.size(), 0);

			if (channel.getTopic().getTopicName().empty()) {
				toSend = "[no topic set]\r\n";
				send(member.getClientSocket(), toSend.c_str(), toSend.size(), 0);
			}
			else {
				toSend = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "Topic for " + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + ": " + channel.getTopic().getTopicName() + "\r\n";
				send(member.getClientSocket(), toSend.c_str(), toSend.size(), 0);
			}
		}
	}

	std::stringstream ss;
	ss << MSG_JOIN_CHAN << channelName << std::endl << std::endl;
	std::string channelJoinedMsg = ss.str();
	client.sendClientMsg(client.getClientSocket(), channelJoinedMsg.c_str());
}

void Server::joinCmdClient(Client& client, const std::vector<std::string>& tokens) {
	if (!validateTokensJoin(client, tokens))
		return;

	std::string channelsNames = trim(tokens[1]);

	std::vector<std::string> channelsList = splitComa(channelsNames);

	for (std::vector<std::string>::iterator it = channelsList.begin(); it != channelsList.end(); ++it)
		handleChannel(client, *it, tokens);

}



// /connect localhost 6667 1

