#include "../../includes/Server.hpp"

/************************************* CONST ET DEST *************************************/

Channel::Channel() {
	_channelName = "random";
}

Channel::Channel(const std::string& channelName) {
	_channelName = channelName;
	_password = "";
	_topicRight = false;
	_invitationAccess = false;
	_maxMembersChannel = 10;
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

int Channel::getMembersCount() const {
		return (_memberSockets.size());
}

const Topic& Channel::getTopic() const {
	return (_topic);
}

Topic& Channel::getTopic() {
	return (_topic);
}

std::string	Channel::getMode() const {
	return (_mode);
}

bool	Channel::getTopicRight() const {
	return (_topicRight);
}

const std::string&	Channel::getChannelKey() const {
	return (_password);
}

int Channel::getMaxMembers() const {
	return (_maxMembersChannel);
}

const std::vector<int>& Channel::getOperators() const {
	return (_membersOperators);
}

const std::vector<int>& Channel::getMembersInvite() const {
	return (_membersIsInvite);
}

bool	Channel::getInvitationAccess() const {
	return (_invitationAccess);
}

/*************************************** SETTERS ***************************************/

void	Channel::setMode(const std::string& mode) {
	_mode = mode;
}

void	Channel::setTopicRight(bool topicRight) {
	_topicRight = topicRight;
}

void	Channel::setMaxMembers(int newMaxMember) {
	_maxMembersChannel = newMaxMember;
}

void	Channel::setChannelKey(std::string password) {
	_password = password;
}

void	Channel::setInvitationAccess(bool invitationAccess) {
	_invitationAccess = invitationAccess;
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

