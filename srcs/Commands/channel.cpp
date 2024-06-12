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
	// Vérifie si le client n'est pas déjà membre du canal
	if (!isMember(clientSocket)) {
		// Ajoute le client à la liste des membres du canal
		_memberSockets.push_back(clientSocket);
	}
}

void Channel::removeMember(int clientSocket) {
	// Parcourt la liste des membres du canal
	std::vector<int>::iterator it;
	for (it = _memberSockets.begin(); it != _memberSockets.end(); ++it) {
		// Si le socket du client correspond à celui fourni, le retire du canal
		if (*it == clientSocket) {
			_memberSockets.erase(it);
			break; // Arrête la boucle une fois que le client est retiré
		}
	}
}

bool Channel::isMember(int clientSocket) const {
	// Parcourt la liste des membres du canal
	std::vector<int>::const_iterator it;
	for (it = _memberSockets.begin(); it != _memberSockets.end(); ++it) {
		// Si le socket du client est trouvé dans la liste, retourne true
		if (*it == clientSocket) {
			return true;
		}
	}
	// Si le socket du client n'est pas trouvé, retourne false
	return false;
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

	if (tokens.size() < 2) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_CHANNEL);
		return;
	}
	
	std::string	channelName = trim(tokens[1]);
	
	if (!checkChannelName(channelName)) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CHANNELNAME);
		return;
	}
	
	Channel channel(channelName);

}

/* WARNING TO DO

	gérer correctement les cas où le même client essaie d'être ajouté plusieurs fois
	ou retiré d'un canal auquel il n'appartient pas.

	nom du canal est unique
 */