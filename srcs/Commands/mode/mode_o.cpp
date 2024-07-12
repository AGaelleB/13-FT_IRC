#include "../../../includes/Server.hpp"

/* 

+o donne les privilèges des opérateurs :
	Changer le topic du canal.
	Expulser des utilisateurs du canal.
	Définir d'autres modes de canal.
	Attribuer ou révoquer le statut d'opérateur à d'autres utilisateurs.
    
 */

void	Server::modeOCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
	if (tokens[2] == "+o")
		modePlusOCmd(client, tokens, channel, channelName);
	else if (tokens[2] == "-o")
		modeMinusdOCmd(client, tokens, channel, channelName);
}

void	Server::modePlusOCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
	if (tokens.size() != 4) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE_O);
		return;
	}

	int userSocket = -1;
    for (std::vector<int>::const_iterator it = channel.getMembers().begin(); it != channel.getMembers().end(); it++) {
        Client& memberClient = getClientBySocket(*it);
        if (memberClient.getUser().getNickname() == tokens[3]) {
            userSocket = memberClient.getClientSocket();
            break;
        }
    }
	if (userSocket != -1) {
		channel.addOperator(userSocket);
		std::string fullMessage = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " +o " + tokens[3] + "\r\n";
		broadcastMessageToChannel(channelName, fullMessage, -1);
	}
	else {
		client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: User " + tokens[3] + " not found in the channel: [" + channelName + "]\n" + RESET).c_str());
		return;
	}
}

void	Server::modeMinusdOCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
	if (tokens.size() < 4) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE);
		return;
	}

	int userSocket = -1;
    for (std::vector<int>::const_iterator it = channel.getMembers().begin(); it != channel.getMembers().end(); it++) {
        Client& memberClient = getClientBySocket(*it);
        if (memberClient.getUser().getNickname() == tokens[3]) {
            userSocket = memberClient.getClientSocket();
            break;
        }
    }
	if (userSocket != -1) {
		channel.removeOperator(userSocket);
		std::string fullMessage = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " -o " + tokens[3] + "\r\n";
		broadcastMessageToChannel(channelName, fullMessage, -1);
	}
	else {
		client.sendClientMsg(client.getClientSocket(), (std::string(RED) + "Error: User " + tokens[3] + " not found in the channel: [" + channelName + "]\n" + RESET).c_str());
		return;
	}
}

void	Channel::addOperator(int clientToAdd) {
	if (!isOperator(clientToAdd))
		_membersOperators.push_back(clientToAdd);
}

void	Channel::removeOperator(int clientToErase) {
	std::vector<int>::iterator it;
	for (it = _membersOperators.begin(); it != _membersOperators.end(); ++it) {
		if (*it == clientToErase) {
			_membersOperators.erase(it);
			break; 
		}
	}
}

bool Channel::isOperator(int clientSocket) const {
	std::vector<int>::const_iterator it;
	for (it = _membersOperators.begin(); it != _membersOperators.end(); ++it) {
		if (*it == clientSocket)
			return (true);
	}
	return (false);
}
