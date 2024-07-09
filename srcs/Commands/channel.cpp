#include "../../includes/Server.hpp"

/************************************* CONST ET DEST *************************************/

Channel::Channel() {
	_channelName = "random";
}

Channel::Channel(const std::string& channelName) {
	_channelName = channelName;
}

Channel::~Channel() {

}


/*************************************** GETTERS ***************************************/

const std::string& Channel::getName() const {
	return (_channelName);
}

const std::vector<int>&	Channel::getMembers() const {
	return (_memberSockets);
}


/************************************** FUNCTIONS **************************************/

void Channel::addMember(int clientSocket) {
	if (!isMember(clientSocket))
		_memberSockets.push_back(clientSocket);
}


void Channel::removeMember(int clientSocket) {
	std::vector<int>::iterator it;
	for (it = _memberSockets.begin(); it != _memberSockets.end(); ++it) {
		if (*it == clientSocket) {
			_memberSockets.erase(it);
			break; 
		}
	}
}

bool Channel::isMember(int clientSocket) const {
	std::vector<int>::const_iterator it;
	for (it = _memberSockets.begin(); it != _memberSockets.end(); ++it) {
		if (*it == clientSocket)
			return (true);
	}
	return (false);
}

bool Server::checkChannelName(const std::string& channelName) {

	if (channelName.length() < MIN_NAME_CHANNEL_SIZE || channelName.length() > MAX_NAME_CHANNEL_SIZE)
		return (false);

	if (channelName[0] != '#')
		return (false);

	bool hasAlnum = false;

	for (size_t i = 1; i < channelName.length(); ++i) {
		if (channelName[i] == '#')
			return (false);
		if (std::isalnum(channelName[i]))
			hasAlnum = true;
		if (!std::isalnum(channelName[i]) && channelName[i] != '_' && channelName[i] != '-' && channelName[i] != ':')
			return (false);
	}
	return (hasAlnum);
}

void Server::createChannel(Client& client, std::string channelName) {
    if (!checkChannelName(channelName)) {
        client.sendClientMsg(client.getClientSocket(), ERROR_CHANNELNAME);
        return;
    }
    Channel channel(channelName);

    std::pair<std::map<std::string, Channel>::iterator, bool> result = _channels.insert(std::make_pair(channelName, channel));
    if (result.second == false) {
        client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_ALREADY_EXIST);
        return;
    }

    _channelOrder.push_back(channelName); // Ajouter le nom du channel à la liste dans l'ordre de création

    std::cout << BOLD << "Channel: [" << channelName << "] created successfully! ✅" << RESET << std::endl;
}

bool Server::validateTokensJoin(Client& client, const std::vector<std::string>& tokens) {
	if (tokens.size() != 2) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_CHANNEL);
		return (false);
	}
	return (true);
}

void Server::handleChannel(Client& client, std::string& channelName) {

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
		it->second.addMember(client.getClientSocket());

		std::string joinMsg = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname JOIN " + channelName + "\r\n";
		broadcastMessageToChannel(channelName, joinMsg, -1);
		
		if (client.isIrssi) {
			std::string membersList = PrintChannelListMembers(channelName, _channels);
			std::string RPL_MsgNames = RPL_NAMREPLY(client.getUser().getNickname(), channelName, membersList);
			client.sendClientMsg(client.getClientSocket(), RPL_MsgNames.c_str());

			std::string RPL_MsgEndOfNames = RPL_ENDOFNAMES(client.getUser().getNickname(), channelName);
			client.sendClientMsg(client.getClientSocket(), RPL_MsgEndOfNames.c_str());
		}

        std::stringstream ss;
        ss << MSG_JOIN_CHAN << channelName << std::endl << std::endl;
        std::string channelJoinedMsg = ss.str();
        client.sendClientMsg(client.getClientSocket(), channelJoinedMsg.c_str());
	}
	else {
        std::stringstream ss;
        ss << MSG_ALREADY_CHAN << channelName << std::endl << std::endl;
        std::string channelMsg = ss.str();
        client.sendClientMsg(client.getClientSocket(), channelMsg.c_str());
	}
}

void Server::joinChannel(Client& client, const std::vector<std::string>& tokens) {
	if (!validateTokensJoin(client, tokens))
		return;

	std::string channelsNames = trim(tokens[1]);

	std::vector<std::string> channelsList = splitComa(channelsNames);

	for (std::vector<std::string>::iterator it = channelsList.begin(); it != channelsList.end(); ++it)
		handleChannel(client, *it);

}


// /connect localhost 6667 1

