void Server::modeCmdClient(Client& client, std::vector<std::string> tokens) {
	if (tokens.size() < 3) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE);
		return;
	}

	std::string channelName = tokens[1];
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end()) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_NOT_FOUND);
		return;
	}

	Channel& channel = it->second;

	std::string mode = tokens[2];
	if (mode == "+i") {
		channel.setInviteOnly(true);
	} else if (mode == "-i") {
		channel.setInviteOnly(false);
	} else if (mode == "+t") {
		channel.setTopicRestricted(true);
	} else if (mode == "-t") {
		channel.setTopicRestricted(false);
	} else if (mode == "+k") {
		if (tokens.size() < 4) {
			client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE);
			return;
		}
		channel.setChannelKey(tokens[3]);
	} else if (mode == "-k") {
		channel.setChannelKey("");
	} else if (mode == "+o") {
		if (tokens.size() < 4) {
			client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE);
			return;
		}
		int userSocket = getUserSocketByName(tokens[3]);
		if (userSocket != -1) {
			channel.addOperator(userSocket);
			// Notify the channel about the new operator
			std::string fullMessage = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " +o " + tokens[3] + "\r\n";
			broadcastMessageToChannel(channelName, fullMessage, -1);
		}
	} else if (mode == "-o") {
		if (tokens.size() < 4) {
			client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE);
			return;
		}
		int userSocket = getUserSocketByName(tokens[3]);
		if (userSocket != -1) {
			channel.removeOperator(userSocket);
			// Notify the channel about the removed operator
			std::string fullMessage = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " -o " + tokens[3] + "\r\n";
			broadcastMessageToChannel(channelName, fullMessage, -1);
		}
	} else if (mode == "+l") {
		if (tokens.size() < 4) {
			client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE);
			return;
		}
		int limit = std::stoi(tokens[3]);
		channel.setUserLimit(limit);
	} else if (mode == "-l") {
		channel.setUserLimit(0);
	} else {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE);
	}
}

int Server::getUserSocketByName(const std::string& userName) const {
	for (const auto& pair : _clients) {
		if (pair.second.getUser().getNickname() == userName) {
			return
