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
        client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_FULL);
        return;
    }

    // Vérifier si le canal est en mode invitation seulement (+i)
    if (channel.getInvitationAccess()) {
        std::cout << YELLOW << "MODE +i true" << RESET << std::endl;
        if (!channel.isInvitedMember(client.getClientSocket())) {
            std::cout << YELLOW << "MODE +i true, not invited" << RESET << std::endl;
            client.sendClientMsg(client.getClientSocket(), ERROR_INVITE_ONLY_CHANNEL);
            return;
        }
    }

    // Ajouter le client au canal
    channel.addMember(client.getClientSocket());
    sendChannelJoinInfo(channel, channelName, client);
}