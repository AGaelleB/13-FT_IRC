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

	// std::cout << YELLOW << "1 checkChannelName channelName =" << channelName <<  RESET << std::endl; 

	for (size_t i = 1; i < channelName.length(); ++i) {
		if (channelName[i] == '#')
			return (false);
		if (std::isalnum(channelName[i]))
			hasAlnum = true;
		if (!std::isalnum(channelName[i]) && channelName[i] != '_' && channelName[i] != '-' && channelName[i] != ':')
			return (false);
	}
	// std::cout << YELLOW << "2 checkChannelName channelName =" << channelName <<  RESET << std::endl; 

	return (hasAlnum);
}

void Server::createChannel(Client& client, std::string channelName) {
	
	std::cout << YELLOW << "START createChannel" << RESET << std::endl; 

	if (!checkChannelName(channelName)) {
		// std::cout << YELLOW << "createChannel ERROR_CHANNELNAME" << RESET << std::endl; 
		// std::cout << YELLOW << "createChannel channelName =" << channelName <<  RESET << std::endl; 
		client.sendClientMsg(client.getClientSocket(), ERROR_CHANNELNAME);
		return;
	}
	Channel channel(channelName);

	// Ajouter le canal dans le container
	std::pair<std::map<std::string, Channel>::iterator, bool> result = _channels.insert(std::make_pair(channelName, channel));
	if (result.second == false) {
		// std::cout << YELLOW << "createChannel ERROR_CHANNEL_ALREADY_EXIST" << RESET << std::endl; 
		client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_ALREADY_EXIST);
		return;
	}

	std::cout << BOLD << "Channel: [" << channelName << "] created successfully! ✅" << RESET << std::endl;
}

void Server::joinChannel(Client& client, const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        client.sendClientMsg(client.getClientSocket(), ERROR_CMD_CHANNEL);
        return;
    }

    std::string channelName = trim(tokens[1]);

    // Ajoute un # au début du nom du canal s'il n'y en a pas déjà un
    if (channelName[0] != '#')
        channelName = "#" + channelName;

    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it == _channels.end()) {
        createChannel(client, channelName);

        usleep(42); // C'est peut-être une attente délibérée, assurez-vous de sa nécessité

        // Recherche à nouveau le canal après la création
        it = _channels.find(channelName);
        if (it == _channels.end()) {
            client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_FAILED_CREATE);
            return;
        }
    }

    // Ajoutez le client au canal
    it->second.addMember(client.getClientSocket());

    // Envoyer un message JOIN à tous les membres du canal
    std::string joinMsg = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname JOIN " + channelName + "\r\n";
    broadcastMessageToChannel(channelName, joinMsg, -1); // -1 pour envoyer à tous les clients, y compris le client actuel

    // Si le client est irssi, envoyer les RPL appropriés
    if (client.isIrssi) {
        std::string membersList = PrintChannelListMembers(channelName, _channels);
        std::string RPL_MsgNames = RPL_NAMREPLY(client.getUser().getNickname(), channelName, membersList);
        client.sendClientMsg(client.getClientSocket(), RPL_MsgNames.c_str());

        std::string RPL_MsgEndOfNames = RPL_ENDOFNAMES(client.getUser().getNickname(), channelName);
        client.sendClientMsg(client.getClientSocket(), RPL_MsgEndOfNames.c_str());
    }

    // Envoyer des messages de confirmation au client
    std::stringstream ss;
    ss << "You are now in the Channel " << channelName << std::endl << std::endl;
    std::string channelJoinedMsg = ss.str();
    client.sendClientMsg(client.getClientSocket(), channelJoinedMsg.c_str());
}





// /connect localhost 6667 1

/* WARNING TO DO

	gérer correctement les cas où le même client essaie d'être retiré d'un canal auquel il n'appartient pas.

 */