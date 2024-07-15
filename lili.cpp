void Server::inviteCmdClient(Client& client, std::vector<std::string> tokens) {
    if (tokens.size() != 3) {
        client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: Invalid number of parameters. Usage: /INVITE <user> <channel>\n" + RESET).c_str());
        return;
    }

    int userSocket = -1;
    Client* invitedClient = NULL;
    std::string fullMessage;

    std::string channelName = tokens[2];

    if (channelName[0] != '#')
        channelName = "#" + channelName;

    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it == _channels.end()) {
        client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: Channel " + channelName + " not found\n" + RESET).c_str());
        return;
    }

    Channel& channel = it->second;
    if (!channel.isMember(client.getClientSocket())) {
        client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: You are not a member of the channel " + channelName + "\n" + RESET).c_str());
        return;
    }

    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getUser().getNickname() == tokens[1]) {
            userSocket = it->second.getClientSocket();
            invitedClient = &it->second;
            break;
        }
    }

    if (userSocket != -1 && invitedClient != NULL) {
        if (channel.isMember(userSocket)) {
            client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: User " + invitedClient->getUser().getNickname() + " is already in the channel " + channelName + "\n" + RESET).c_str());
            return;
        }

        channel.addInvitedMember(userSocket);
        std::string inviteMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + client.getUser().getNickname() + " invited " + invitedClient->getUser().getNickname() + " to join " + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "\r\n";
        std::string inviteMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname INVITE " + tokens[1] + " :" + channelName + "\r\n";

        const std::vector<int>& members = channel.getMembers();
        for (size_t i = 0; i < members.size(); ++i) {
            int memberSocket = members[i];
            Client& memberClient = _clients[memberSocket];

            if (memberClient.isIrssi)
                fullMessage = inviteMsgIrssi;
            else
                fullMessage = inviteMsgNetcat;
            ::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
        }
        invitedClient->sendClientMsg(userSocket, ("You have been invited to join " + channelName + " by " + client.getUser().getNickname() + "\r\n").c_str());
    } else {
        client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: User " + tokens[1] + " not found\n" + RESET).c_str());
        return;
    }
}

void Channel::addInvitedMember(int clientToAdd) {
    if (!isInvitedMember(clientToAdd))
        _membersOperators.push_back(clientToAdd);
}

bool Channel::isInvitedMember(int clientSocket) const {
    std::vector<int>::const_iterator it;
    for (it = _membersOperators.begin(); it != _membersOperators.end(); ++it) {
        if (*it == clientSocket)
            return true;
    }
    return false;
}
