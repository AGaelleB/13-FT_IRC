void Server::topicCmdClient(Client& client, std::vector<std::string> tokens) {
    if (!validateTokensTopic(client, tokens))
        return;

    std::string channelName = tokens[1];

    if (channelName[0] != '#')
        channelName = "#" + channelName;

    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it == _channels.end()) {
        client.sendClientMsg(client.getClientSocket(), ERR_NOSUCHCHANNEL(channelName).c_str());
        return;
    }

    Channel& channel = it->second;

    if (tokens.size() == 2) {
        topicInfo(client, channel, channelName);
    } else {
        if (channel.isMember(client.getClientSocket())) {
            if (channel.getTopicRight()) {
                if (channel.isOperator(client.getClientSocket())) {
                    topicSetUp(client, channel, channelName, tokens);
                } else {
                    client.sendClientMsg(client.getClientSocket(), ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName).c_str());
                    return;
                }
            } else {
                topicSetUp(client, channel, channelName, tokens);
            }
        } else {
            client.sendClientMsg(client.getClientSocket(), ERR_NOTONCHANNEL(client.getNickname(), channelName).c_str());
        }
    }
}


// Topic messages
std::string RPL_TOPIC(const std::string &client, const std::string &channel, const std::string &topic) {
	return ":localhost 332 " + client + " " + channel + " :" + topic + "\r\n";
}

std::string RPL_NOTOPIC(const std::string &client, const std::string &channel) {
	return ":localhost 331 " + client + " " + channel + " :No topic is set\r\n";
}

peut tui aussi paramettrer le RPL 
if (channel.getTopic().getTopicName().empty()) {
				toSend = "[no topic set]\r\n";
			}



        if (channel.getTopic().getTopicName().empty()) {
            client.sendClientMsg(client.getClientSocket(), RPL_NOTOPIC(client.getUser().getNickname(), channelName).c_str());
