#include "../../includes/Server.hpp"

/* 
	/TOPIC <channel> OR /TOPIC <channel> <new topic>
 */

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
		std::string errorMsgNetcat = std::string(RED) + "Error: Invalid number of parameters. Usage: /TOPIC <channel> OR /TOPIC <channel> <new topic>\n" + std::string(RESET);
		std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :Invalid number of parameters. Usage: /TOPIC <channel> OR /TOPIC <channel> <new topic>\r\n";
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return (false);
	}
	return (true);
}

void Server::topicInfo(Client& client, Channel& channel, std::string channelName) {
	if (channel.getTopic().getTopicName().empty()) {
		std::string noTopic = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "No topic set for " + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "\r\n";
		sendErrorMessage(client, noTopic, noTopic);
	}
	else {
		std::string topicSet = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "Topic for " + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + ": " + channel.getTopic().getTopicName() + "\r\n";
		sendErrorMessage(client, topicSet, topicSet);
	}
}

void Server::topicSetUp(Client& client, Channel& channel, std::string channelName, std::vector<std::string> tokens) {
	std::string newTopic = tokens[2];
	for (size_t i = 3; i < tokens.size(); ++i) {
		newTopic += " " + tokens[i];
	}

	if (newTopic.size() > MAX_TOPIC_SIZE || newTopic.size() < MIN_TOPIC_SIZE) {
		std::ostringstream oss;
		oss << "Error: Topic size must be between " << MIN_TOPIC_SIZE << " and " << MAX_TOPIC_SIZE << " characters.\n";
		std::string errorMsgNetcat = std::string(RED) + oss.str() + std::string(RESET);
		std::ostringstream ossIrssi;
		ossIrssi << ":" << client.getUser().getNickname() << "!" << client.getUser().getUsername() << "@hostname NOTICE " << client.getUser().getNickname() << " :Topic size must be between " << MIN_TOPIC_SIZE << " and " << MAX_TOPIC_SIZE << " characters.\r\n";
		std::string errorMsgIrssi = ossIrssi.str();
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return;
	}
	else {
		if (newTopic == ":") {
			newTopic = "";
			std::string noTopic = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "No topic set for " + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "\r\n";
			sendErrorMessage(client, noTopic, noTopic);
			return;
		}
		channel.getTopic().setTopicName(newTopic);

		std::string topicMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "Topic for " + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + ": " + newTopic + "\r\n";
		std::string topicMsgIrssi = RPL_TOPIC(client.getUser().getNickname(), channelName,  channel.getTopic().getTopicName()).c_str();

		const std::vector<int>& members = channel.getMembers();
		for (size_t i = 0; i < members.size(); ++i) {
			int memberSocket = members[i];
			Client& memberClient = _clients[memberSocket];

			std::string fullMessage;
			if (memberClient.isIrssi)
				fullMessage = topicMsgIrssi;
			else 
				fullMessage = topicMsgNetcat;
			::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
		}
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
		std::string errorMsgNetcat = std::string(RED) + "Error: No such channel " + channelName + "\n" + std::string(RESET);
		std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :No such channel " + channelName + "\r\n";
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return;
	}

	Channel& channel = it->second;

	if (tokens.size() == 2)
		topicInfo(client, channel, channelName);
	else {
		if (channel.isMember(client.getClientSocket())) {
			if (channel.getTopicRight()) {
				if (channel.isOperator(client.getClientSocket()))
					topicSetUp(client, channel, channelName, tokens);
				else {
					std::string errorMsgNetcat = std::string(RED) + "Error: You're not channel operator for " + channelName + "\n" + std::string(RESET);
					std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :You're not channel operator for " + channelName + "\r\n";
					sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
					return;
				}
			}
			else
				topicSetUp(client, channel, channelName, tokens);
		}
		else {
			std::string errorMsgNetcat = std::string(RED) + "Error: You're not in the channel " + channelName + "\n" + std::string(RESET);
			std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :You're not in the channel " + channelName + "\r\n";
			sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		}
	}
}
