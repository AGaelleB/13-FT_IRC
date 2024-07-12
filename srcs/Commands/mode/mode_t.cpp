#include "../../../includes/Server.hpp"

void Server::modeTCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
	if (tokens.size() != 3) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE_T); 
		return;
	}

	std::string topicRightMsgNetcat;
	std::string topicRightMsgIrssi;
	std::string fullMessage;

	if (tokens[2] == "+t") {
		channel.setTopicRight(true);
		topicRightMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "mode/" + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "[+t] by " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "\r\n";
		topicRightMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " +t\r\n";
	}
	else if (tokens[2] == "-t") {
		channel.setTopicRight(false);
		topicRightMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "mode/" + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "[-t] by " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "\r\n";
		topicRightMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " -t\r\n";
	}
	else {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE_T);
		return;
	}

	const std::vector<int>& members = channel.getMembers();
	for (size_t i = 0; i < members.size(); ++i) {
		int memberSocket = members[i];
		Client& memberClient = _clients[memberSocket];

		if (memberClient.isIrssi)
			fullMessage = topicRightMsgIrssi;
		else
			fullMessage = topicRightMsgNetcat;
		::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
	}
}



// /connect localhost 6667 1
