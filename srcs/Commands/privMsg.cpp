#include "../../includes/Server.hpp"

bool Server::validateTokensPrivMsg(Client& client, const std::vector<std::string>& tokens) {
	if (tokens.size() < 3) {
		std::string irssiMessage = ERR_NEEDMOREPARAMS(client.getUser().getNickname(), "PRIVMSG");
		std::string netcatMessage = "Error: Must be: /msg OR /PRIVMSG <target> <message>\n";
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return false;
	}
	return true;
}

std::string Server::extractMessageContent(const std::string& message, const std::string& targets) {
	std::string msgContent;
	size_t pos = message.find(":", message.find(targets));

	if (pos != std::string::npos)
		msgContent = message.substr(pos + 1);
	else
		msgContent = message.substr(message.find(targets) + targets.length() + 1);

	return (msgContent);
}

void Server::sendMessageToUser(Server& server, Client& client, const std::string& target, const std::string& msgContent) {
	int recipientSocket = -1;
	for (std::map<int, Client>::iterator it = server._clients.begin(); it != server._clients.end(); ++it) {
		if (it->second.getUser().getNickname() == target) {
			recipientSocket = it->second.getClientSocket();
			break;
		}
	}

	if (recipientSocket == -1) {
		std::string irssiMessage = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :User " + target + " not found\r\n";
		std::string netcatMessage = "Error: User " + target + " not found\n";
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}

	Client& recipientClient = server._clients[recipientSocket];

	std::string fullMessage;
	if (recipientClient.isIrssi)
		fullMessage = ":" + client.getUser().getNickname() + " PRIVMSG " + target + " :" + msgContent + "\r\n";
	else
		fullMessage = "<" + client.getUser().getNickname() + "> " + msgContent + "\r\n";

	::send(recipientSocket, fullMessage.c_str(), fullMessage.size(), 0);

	std::string logMessage = "<" + client.getUser().getNickname() + "> PRIVMSG " + target + ": " + msgContent;
	std::cout << logMessage << std::endl;
}

void Server::handleTarget(Server& server, Client& client, const std::string& target, const std::string& msgContent) {
	if (target[0] == '#')
		sendMessageToChannel(server, client, target, msgContent);
	else
		sendMessageToUser(server, client, target, msgContent);
}

void Server::privMsgCmdClient(Client& client, const std::vector<std::string>& tokens, const std::string& message) {
	if (!validateTokensPrivMsg(client, tokens))
		return;

	std::string targets = tokens[1];
	std::string msgContent = extractMessageContent(message, targets);

	if (msgContent.length() > MAX_SIZE_MSG) {
		std::string irssiMessage = ":localhost 404 " + client.getUser().getNickname() + " :Message too long\r\n";
		std::string netcatMessage = "Error: Message too long\n";
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}

	std::vector<std::string> targetList = splitComa(targets);

	for (std::vector<std::string>::iterator it = targetList.begin(); it != targetList.end(); ++it)
		handleTarget(*this, client, *it, msgContent);
}


void Server::sendMessageToChannel(Server& server, Client& client, const std::string& channelName, const std::string& msgContent) {
	std::map<std::string, Channel>::iterator it = server._channels.find(channelName);
	if (it == server._channels.end()) {
		std::string irssiMessage = ERR_NOSUCHCHANNEL(channelName);
		std::string netcatMessage = std::string(RED) + "Error: Channel " + channelName + " not found\n" + RESET;
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}

	if (!it->second.isMember(client.getClientSocket())) {
		std::string irssiMessage = ERR_NOTONCHANNEL(client.getUser().getNickname(), channelName);
		std::string netcatMessage = std::string(RED) + "Error: You are not a member of the channel " + channelName + "\n" + RESET;
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}

	Channel& channel = it->second;

	// Vérifier si le message contient des mots interdits
	if (checkBannedWordsPrivMsg(client, channel, channelName, msgContent))
		return;

	const std::vector<int>& members = channel.getMembers();
	for (size_t i = 0; i < members.size(); ++i) {
		int memberSocket = members[i];
		Client& memberClient = server._clients[memberSocket];

		std::string fullMessage;
		if (memberClient.isIrssi && client.getClientSocket() != memberClient.getClientSocket())
			fullMessage = ":" + client.getUser().getNickname() + " PRIVMSG " + channelName + " :" + msgContent + "\r\n";
		else if (client.getClientSocket() == memberClient.getClientSocket())
			fullMessage = "[" + channelName + "] <" + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "> " + msgContent + "\r\n";
		else
			fullMessage = "[" + channelName + "] <" + client.getUser().getNickname() + "> " + msgContent + "\r\n";

		::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
	}
}


// /connect localhost 6667 1