#include "../../includes/Server.hpp"

/* 
	KICK <channel> <user> [<comment>]
 */

// /connect localhost 6667 1

void Server::kickCmdClientReason(Client& client, Channel& channel, std::vector<std::string> tokens, int userSocket) {
	std::string reason = "No reason specified";
	if (tokens.size() > 3) 
		reason = joinTokens(tokens, 3);

	executeKick(client, channel, reason, userSocket);
}

void Server::executeKick(Client& client, Channel& channel, const std::string& reason, int userSocket) {
	// Récupère le client à partir du socket utilisateur
	Client& targetClient = _clients[userSocket];

	// Retire le membre du canal
	channel.removeMember(userSocket);
	if (channel.isOperator(userSocket))
		channel.removeOperator(userSocket);
	if (channel.isInvitedMember(userSocket))
		channel.removeInvitedMember(userSocket);

	std::string channelName = channel.getName();
	if (channelName[0] != '#')
		channelName = "#" + channelName;
	std::string kickMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + std::string(CYAN_IRSSI) + client.getUser().getNickname() + std::string(RESET) + " [" + std::string(CYAN_IRSSI) + client.getUser().getUsername() + "@localhost" + std::string(RESET) + "] has kicked " + std::string(BOLD) + targetClient.getUser().getNickname() + std::string(RESET) + " from " + std::string(BOLD) + channelName + std::string(RESET) + " [" + reason + "]\r\n";
	std::string kickMsgIrssi = RPL_KICK(client.getUser().getNickname(), client.getUser().getUsername(), channelName, targetClient.getUser().getNickname(), reason);

	// Diffuser le message de départ à tous les membres du canal
	const std::vector<int>& members = channel.getMembers();
	for (size_t i = 0; i < members.size(); ++i) {
		int memberSocket = members[i];
		Client& memberClient = _clients[memberSocket];

		std::string fullMessage;
		if (memberClient.isIrssi)
			fullMessage = kickMsgIrssi;
		else 
			fullMessage = kickMsgNetcat;
		::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
	}

	// Envoyer un message de notification à l'utilisateur kické
	std::string notificationNetcat = std::string(RED) + "You have been kicked by " + client.getUser().getNickname() + " from channel " + channelName + " [" + reason + "]\n" + RESET;
	std::string notificationIrssi = RPL_KICK(client.getUser().getNickname(), client.getUser().getUsername(), channelName, targetClient.getUser().getNickname(), reason);

	if (targetClient.isIrssi)
		targetClient.sendClientMsg(targetClient.getClientSocket(), notificationIrssi.c_str());
	else
		targetClient.sendClientMsg(targetClient.getClientSocket(), notificationNetcat.c_str());
}

void Server::kickCmdClient(Client& client, std::vector<std::string> tokens) {
	// Vérifie si le nombre de paramètres est suffisant pour la commande KICK
	if (tokens.size() < 3) {
		std::string errorMsgNetcat = std::string(RED) + "Error: Invalid number of parameters. Usage: /KICK <channel> <user> [<comment>]\n" + RESET;
		std::string errorMsgIrssi = ERR_NEEDMOREPARAMS(client.getUser().getNickname(), "KICK");
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return;
	}

	// Vérifie si le nom du canal commence par '#', sinon l'ajoute
	std::string channelName = tokens[1];
	if (channelName[0] != '#')
		channelName = "#" + channelName;

	// Vérifie si le canal existe
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end()) {
		std::string errorMsgNetcat = std::string(RED) + "Error: Channel " + channelName + " not found\n" + RESET;
		std::string errorMsgIrssi = ERR_NOSUCHCHANNEL(channelName);
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return;
	}

	Channel& channel = it->second;

	// Vérifie si le client est un opérateur du canal
	if (!channel.isOperator(client.getClientSocket())) {
		std::string errorMsgNetcat = std::string(RED) + "Error: You must be a channel operator to use this command\n" + RESET;
		std::string errorMsgIrssi = ERR_CHANOPRIVSNEEDED(client.getUser().getNickname(), channelName);
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return;
	}

	// Trouve le socket de l'utilisateur à kicker
	// std::string nickName = tokens[2];
	// if (nickName[0] == ':')
	// 	nickName = nickName.substr(1); 
	int userSocket = findUserSocket(tokens[2]);
	if (userSocket == -1) {
		std::string errorMsgNetcat = std::string(RED) + "Error: User " + tokens[2] + " not found\n" + RESET;
		std::string errorMsgIrssi = ERR_NOSUCHNICK(client.getUser().getNickname(), tokens[2]);
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return;
	}

	// Vérifie si l'utilisateur est membre du canal
	if (!channel.isMember(userSocket)) {
		std::string errorMsgNetcat = std::string(RED) + "Error: User " + tokens[2] + " is not in the channel " + channelName + "\n" + RESET;
		std::string errorMsgIrssi = ERR_USERNOTINCHANNEL(client.getUser().getNickname(), tokens[2], channelName);
		sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
		return;
	}

	// Appelle la fonction pour gérer le kick avec la raison
	kickCmdClientReason(client, channel, tokens, userSocket);
}
