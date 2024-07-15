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
	if (channelName[0] != '#')
		channelName = "#" + channelName;

	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end()) {
		createChannel(client, channelName);
		usleep(42);

		it = _channels.find(channelName);
		if (it == _channels.end()) {
			client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_FAILED_CREATE);
			return;
		}
	}

	if (!it->second.isMember(client.getClientSocket())) {
		if (it->second.getChannelKey() != "") {
			if (tokens[3] != it->second.getChannelKey()) {
				client.sendClientMsg(client.getClientSocket(), ERROR_WRONG_CHANNEL_PASS);
				return;
			}
		}
		else {
			if (tokens.size() != 2) {
				client.sendClientMsg(client.getClientSocket(), ERROR_NO_PASS_CHANNEL);
				return;
			}
		}
		if (it->second.getMembersCount() >= it->second.getMaxMembers()) {
			client.sendClientMsg(client.getClientSocket(), ERROR_NO_PASS_CHANNEL); // channel has full up faire l'erreur  erreur
			return;
		}
		it->second.addMember(client.getClientSocket());
		sendChannelJoinInfo(it->second, channelName, client);
	}
	else {
		std::stringstream ss;
		ss << MSG_ALREADY_CHAN << channelName << std::endl << std::endl;
		std::string channelMsg = ss.str();
		client.sendClientMsg(client.getClientSocket(), channelMsg.c_str());
	}
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

void Server::joinChannel(Client& client, const std::vector<std::string>& tokens) {
	if (!validateTokensJoin(client, tokens))
		return;

	std::string channelsNames = trim(tokens[1]);

	std::vector<std::string> channelsList = splitComa(channelsNames);

	for (std::vector<std::string>::iterator it = channelsList.begin(); it != channelsList.end(); ++it)
		handleChannel(client, *it, tokens);

}


// /connect localhost 6667 1

