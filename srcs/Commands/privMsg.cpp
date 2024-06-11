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

	if (msgContent.length() > 150) {
		client.sendClientMsg(client.getClientSocket(), "Error: Your message exceeds the maximum allowed length of 150 characters.\n");
		return;
	}

	int recipientSocket = -1;
	// Trouver le client destinataire par son surnom
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.getUser().getNickname() == target) {
			recipientSocket = it->second.getClientSocket();
			break;
		}
	}
	// Si le destinataire n'est pas trouvé, envoyer une erreur au client expéditeur
	if (recipientSocket == -1) {
		client.sendClientMsg(client.getClientSocket(), ERROR_TARGET_NOT_FOUND);
		return;
	}

	// Construire le message à envoyer
	std::string fullMessage = std::string(BOLD) + "Msg from " + client.getUser().getNickname() + ": " + std::string(RESET) + msgContent + "\n";

	// Envoyer le message au client destinataire
	client.sendClientMsg(recipientSocket, fullMessage.c_str());
}


// /connect localhost 6667 1