#include "../../includes/Server.hpp"

void Server::privMsgCmdClient(Client& client, const std::vector<std::string>& tokens, const std::string& message) {
	if (tokens.size() < 3) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_PRIVMSG);
		return;
	}

	std::string targets = tokens[1];
	std::string msgContent;
	size_t pos = message.find(":", message.find(targets));

	if (pos != std::string::npos)
		msgContent = message.substr(pos + 1);
	else
		msgContent = message.substr(message.find(targets) + targets.length() + 1);

	if (msgContent.length() > MAX_SIZE_MSG) {
		client.sendClientMsg(client.getClientSocket(), ERROR_MSG_TOO_LONG);
		return;
	}

	std::stringstream ss(targets);
	std::string target;
	while (std::getline(ss, target, ',')) {
		target = trim(target);  // Assurez-vous de supprimer les espaces autour du nom de la cible

		// Vérifier si la cible est un canal ou un utilisateur
		if (target[0] == '#') {
			// Cible est un canal
			std::map<std::string, Channel>::iterator it = _channels.find(target);
			if (it == _channels.end()) {
				client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: Channel " + target + " not found\n" + RESET).c_str());
				continue;
			}

			// Vérifier si le client est membre du canal
			if (!it->second.isMember(client.getClientSocket())) {
				client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: You are not a member of the channel " + target + "\n" + RESET).c_str());
				continue;
			}

			// Envoyer le message à tous les membres du canal
			std::string fullMessage = std::string(BOLD) + "<" + client.getUser().getNickname() + "> PRIVMSG: " + RESET + msgContent + "\r\n";
			broadcastMessageToChannel(target, fullMessage, client.getClientSocket());

		}
		else {
			// Cible est un utilisateur
			int recipientSocket = -1;
			for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
				if (it->second.getUser().getNickname() == target) {
					recipientSocket = it->second.getClientSocket();
					break;
				}
			}
			if (recipientSocket == -1) {
				client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: User " + target + " not found\n" + RESET).c_str());
				continue;
			}

			std::string fullMessage = std::string(BOLD) + "<" + client.getUser().getNickname() + "> PRIVMSG: " + RESET + msgContent + "\r\n";
			client.sendClientMsg(recipientSocket, fullMessage.c_str());
			std::string logMessage = std::string(BOLD) + "<" + client.getUser().getNickname() + ">:" + std::string(RESET) + " PRIVMSG " + target + ":" + msgContent;
			std::cout << logMessage << std::endl;
		}
	}
}


// /connect localhost 6667 1
// /join server
// /msg #server coucou a tous

