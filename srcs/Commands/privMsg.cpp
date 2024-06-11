#include "../../includes/Server.hpp"

void Server::privMsgCmdClient(Client& client, const std::vector<std::string>& tokens, const std::string& message) {
	
	if (tokens.size() < 3) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_PRIVMSG);
		return;
	}
	
	std::string	target = tokens[1];
	std::string	msgContent;
	size_t		pos = message.find(":", message.find(target));

	if (pos != std::string::npos)
		msgContent = message.substr(pos + 1);
	else
		msgContent = message.substr(message.find(target) + target.length() + 1);

	if (msgContent.length() > MAX_SIZE_MSG) {
		client.sendClientMsg(client.getClientSocket(), ERROR_MSG_TOO_LONG);
		return;
	}

	int recipientSocket = -1;
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.getUser().getNickname() == target) {
			recipientSocket = it->second.getClientSocket();
			break;
		}
	}
	if (recipientSocket == -1) {
		client.sendClientMsg(client.getClientSocket(), ERROR_TARGET_NOT_FOUND);
		return;
	}
	std::string fullMessage = std::string(BOLD) + "Msg from " + client.getUser().getNickname() + ": " + std::string(RESET) + msgContent + "\n";
	client.sendClientMsg(recipientSocket, fullMessage.c_str());
}


// /connect localhost 6667 1