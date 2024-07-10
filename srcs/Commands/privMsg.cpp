#include "../../includes/Server.hpp"

bool Server::validateTokensPrivMsg(Client& client, const std::vector<std::string>& tokens) {
	if (tokens.size() < 3) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_PRIVMSG);
		return (false);
	}
	return (true);
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

void Server::sendMessageToChannel(Server& server, Client& client, const std::string& channelName, const std::string& msgContent) {
    std::map<std::string, Channel>::iterator it = server._channels.find(channelName);
    if (it == server._channels.end()) {
        client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: Channel " + channelName + " not found\n" + RESET).c_str());
        return;
    }

    if (!it->second.isMember(client.getClientSocket())) {
        client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: You are not a member of the channel " + channelName + "\n" + RESET).c_str());
        return;
    }

    Channel& channel = it->second;
    const std::vector<int>& members = channel.getMembers();
    for (size_t i = 0; i < members.size(); ++i) {
        int memberSocket = members[i];
        Client& memberClient = server._clients[memberSocket];

        std::string fullMessage;
        if (memberClient.isIrssi && client.getClientSocket() != memberClient.getClientSocket())
            fullMessage = ":" + client.getUser().getNickname() + " PRIVMSG " + channelName + " :" + msgContent + "\r\n";
		else if (client.getClientSocket() == memberClient.getClientSocket())
			fullMessage = "[" + channelName + "] < " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "> " + msgContent + "\r\n";
        else
            fullMessage = "[" + channelName + "] < " + client.getUser().getNickname() + "> " + msgContent + "\r\n";
		
        ::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
    }
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
		client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: User " + target + " not found\n" + RESET).c_str());
		return;
	}

	Client& recipientClient = server._clients[recipientSocket];

	std::string fullMessage;
	if (recipientClient.isIrssi)
		fullMessage = ":" + client.getUser().getNickname() + " PRIVMSG " + target + " :" + msgContent + "\r\n";
	else
		fullMessage = "< " + client.getUser().getNickname() + "> " + msgContent + "\r\n";

	client.sendClientMsg(recipientSocket, fullMessage.c_str());

	std::string logMessage = std::string(BOLD) + "<" + client.getUser().getNickname() + ">:" + std::string(RESET) + " PRIVMSG " + target + ":" + msgContent;
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
		client.sendClientMsg(client.getClientSocket(), ERROR_MSG_TOO_LONG);
		return;
	}

	std::vector<std::string> targetList = splitComa(targets);

	for (std::vector<std::string>::iterator it = targetList.begin(); it != targetList.end(); ++it)
		handleTarget(*this, client, *it, msgContent);
}


// /connect localhost 6667 1


