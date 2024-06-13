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

	for (size_t i = 0; i < channelName.length(); ++i) {
		if (!std::isalnum(channelName[i]) && channelName[i] != '_' && channelName[i] != '-')
			return (false);
	}
	return (true);
}

void Server::createChannel(Client& client, const std::vector<std::string>& tokens) {
	
	std::string	channelName = trim(tokens[1]);
	
	if (!checkChannelName(channelName)) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CHANNELNAME);
		return;
	}
	
	Channel channel(channelName);

	// Ajouter le canal dans le container
	std::pair<std::map<std::string, Channel>::iterator, bool> result = _channels.insert(std::make_pair(channelName, channel));
	if (result.second == false) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_ALREADY_EXIST);
		return;
	}

	std::cout << BOLD << "Channel: [" << channelName << "] created successfully! ✅" << RESET << std::endl;
}

void Server::joinChannel(Client& client, const std::vector<std::string>& tokens) {

	if (tokens.size() < 2) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_CHANNEL);
		return;
	}

	std::string channelName = trim(tokens[1]);

	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end()) {
		createChannel(client, tokens);
		
		// Recherche à nouveau le canal après la création
		it = _channels.find(channelName);
		if (it == _channels.end()) {
			client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_FAILED_CREATE);
			return;
		}
	}

	// Ajoutez le client au canal
	it->second.addMember(client.getClientSocket());

	std::stringstream ss;
	ss << GREEN "You are now in the Channel [" << channelName << "]" << RESET << std::endl;
	std::string channelJoinedMsg = ss.str();
	client.sendClientMsg(client.getClientSocket(), channelJoinedMsg.c_str());
}



/* WARNING TO DO

	gérer correctement les cas où le même client essaie d'être retiré d'un canal auquel il n'appartient pas.

 */