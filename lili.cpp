void Server::sendErrorMessage(Client& client, const std::string& errorMsgNetcat, const std::string& errorMsgIrssi) {
    if (client.isIrssi) {
        ::send(client.getClientSocket(), errorMsgIrssi.c_str(), errorMsgIrssi.size(), 0);
    }
    else {
        ::send(client.getClientSocket(), errorMsgNetcat.c_str(), errorMsgNetcat.size(), 0);
    }
}

int Server::findUserSocket(const std::string& nickname) {
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.getUser().getNickname() == nickname) {
            return it->second.getClientSocket();
        }
    }
    return -1;
}

bool Server::validateInviteCommand(Client& client, const std::vector<std::string>& tokens) {
    if (tokens.size() != 3) {
        std::string errorMsgNetcat = "Error: Invalid number of parameters. Usage: /INVITE <user> <channel>\n";
        std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :Invalid number of parameters. Usage: /INVITE <user> <channel>\r\n";
        sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
        return false;
    }
    return true;
}

bool Server::validateChannelMembership(Client& client, const std::string& channelName) {
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it == _channels.end()) {
        std::string errorMsgNetcat = "Error: Channel " + channelName + " not found\n";
        std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :Channel " + channelName + " not found\r\n";
        sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
        return false;
    }

    Channel& channel = it->second;
    if (!channel.isMember(client.getClientSocket())) {
        std::string errorMsgNetcat = "Error: You are not a member of the channel " + channelName + "\n";
        std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :You are not a member of the channel " + channelName + "\r\n";
        sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
        return false;
    }
    return true;
}

void Server::sendInviteMessages(Client& client, Channel& channel, const std::string& channelName, Client& invitedClient, int userSocket) {
    std::string inviteMsgNetcat = client.getUser().getNickname() + " invited " + invitedClient.getUser().getNickname() + " to join " + channelName + "\r\n";
    std::string inviteMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname INVITE " + invitedClient.getUser().getNickname() + " :" + channelName + "\r\n";

    const std::vector<int>& members = channel.getMembers();
    for (size_t i = 0; i < members.size(); ++i) {
        int memberSocket = members[i];
        Client& memberClient = _clients[memberSocket];

        std::string fullMessage;
        if (memberClient.isIrssi) {
            fullMessage = inviteMsgIrssi;
        } else {
            fullMessage = inviteMsgNetcat;
        }
        ::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
    }

    std::string invitedMessage = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + invitedClient.getUser().getNickname() + " :You have been invited to join " + channelName + " by " + client.getUser().getNickname() + "\r\n";
    ::send(userSocket, invitedMessage.c_str(), invitedMessage.size(), 0);
}

void Server::inviteCmdClient(Client& client, std::vector<std::string> tokens) {
    if (!validateInviteCommand(client, tokens)) {
        return;
    }

    std::string channelName = tokens[2];
    if (channelName[0] != '#') {
        channelName = "#" + channelName;
    }

    if (!validateChannelMembership(client, channelName)) {
        return;
    }

    int userSocket = findUserSocket(tokens[1]);
    if (userSocket == -1) {
        std::string errorMsgNetcat = "Error: User " + tokens[1] + " not found\n";
        std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :User " + tokens[1] + " not found\r\n";
        sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
        return;
    }

    Client& invitedClient = _clients[userSocket];
    Channel& channel = _channels[channelName];

    if (channel.isMember(userSocket)) {
        std::string errorMsgNetcat = "Error: User " + invitedClient.getUser().getNickname() + " is already in the channel " + channelName + "\n";
        std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :User " + invitedClient.getUser().getNickname() + " is already in the channel " + channelName + "\r\n";
        sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
        return;
    }

    channel.addInvitedMember(userSocket);
    sendInviteMessages(client, channel, channelName, invitedClient, userSocket);
}