#include "../../includes/Server.hpp"

/************************************* CONST ET DEST *************************************/

Topic::Topic() {
	_topicName = "";
}

Topic::~Topic() {

}

/*************************************** GETTERS ***************************************/

const std::string& Topic::getTopicName() const {
	return (_topicName);
}

/*************************************** SETTERS ***************************************/

void Topic::setTopicName(const std::string& topicName) {
	_topicName = topicName;
}

/************************************** FUNCTIONS **************************************/

bool Server::validateTokensTopic(Client& client, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_TOPIC);
		return (false);
	}
	return (true);
}

void Server::topicInfo(Client& client, Channel& channel, std::string channelName) {
	if (channel.getTopic().getTopicName().empty()) {
		std::string noTopic = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "No topic set for " + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "\r\n";
		client.sendClientMsg(client.getClientSocket(), noTopic.c_str());
	}
	else {
		std::string TopicSet = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "Topic for " + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + ": " + channel.getTopic().getTopicName() + "\r\n";
		client.sendClientMsg(client.getClientSocket(), TopicSet.c_str());
	}
}

void Server::topicSetUp(Client& client, Channel& channel, std::string channelName, std::vector<std::string> tokens) {
	std::string newTopic = tokens[2];
	for (size_t i = 3; i < tokens.size(); ++i) {
		newTopic += " " + tokens[i];
	}


	// // ATTENTION PROTECTION A FAIRE LORSQUE MODE SERA SET, C-A-D SEULS LES ADMINS PEUVENT MODIF UN TOPIC
	// if (!channel.getTopic().isOperator(client.getClientSocket())) {
	// 	client.sendClientMsg(client.getClientSocket(), ERR_CHANOPRIVSNEEDED(channelName));
	// 	return;
	// }

	if (newTopic.size() > MAX_TOPIC_SIZE || newTopic.size() < MIN_TOPIC_SIZE) {
		client.sendClientMsg(client.getClientSocket(), ERROR_TOPIC_SIZE);
		return;
	}
	else {
		channel.getTopic().setTopicName(newTopic);
		std::string fullMessage = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "Topic for " + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + ": " + newTopic + "\r\n";
		broadcastMessageToChannel(channelName, fullMessage, -1);
	}
}

void Server::topicCmdClient(Client& client, std::vector<std::string> tokens) {
	if (!validateTokensTopic(client, tokens))
		return;

	std::string channelName = tokens[1];

	if (channelName[0] != '#')
		channelName = "#" + channelName;

	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end()) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CHANNEL_NOT_FOUND);
		return;
	}

	Channel& channel = it->second;

	if (tokens.size() == 2)
		topicInfo(client, channel, channelName);
	else {
		if (channel.isMember(client.getClientSocket()))
			topicSetUp(client, channel, channelName, tokens);
		else
			client.sendClientMsg(client.getClientSocket(), ERROR_NOT_IN_CHANNEL_TOPIC);
	}
}

/* 
	proteger si on est pas dasn le channel on ne peut pas le modifier 

 */


// /connect localhost 6667 1
