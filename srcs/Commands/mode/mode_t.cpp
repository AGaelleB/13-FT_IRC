#include "../../../includes/Server.hpp"

/* 
	— t : Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux
	/MODE <channel> <+t / -t>

 */

void Server::modeTCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
	if (tokens.size() != 3) {
		std::string netcatMessage = std::string(RED) + "Error: Must be: /MODE <channel> <+t / -t>\r\n" + std::string(RESET);
		std::string irssiMessage = ":localhost 461 " + client.getUser().getNickname() + " MODE_T :Not enough parameters\r\n";
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}

	if (!channel.isOperator(client.getClientSocket())) {
		std::string errorMessage = std::string(RED) + "Error: Only channel operators can use this mode.\r\n" + std::string(RESET);
		std::string irssiMessage = ":localhost 482 " + client.getUser().getNickname() + " " + channel.getName() + " :You're not channel operator\r\n";
		sendErrorMessage(client, errorMessage, irssiMessage);
		return;
	}

	std::string topicRightMsgNetcat;
	std::string topicRightMsgIrssi;
	std::string fullMessage;

	if (tokens[2] == "+t") {
		channel.setTopicRight(true);
		topicRightMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "mode/" + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "[+t] by " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "\r\n";
		topicRightMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " +t\r\n";
	}
	else if (tokens[2] == "-t") {
		channel.setTopicRight(false);
		topicRightMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "mode/" + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "[-t] by " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "\r\n";
		topicRightMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " -t\r\n";
	}
	else {
		std::string netcatMessage = "Error: Must be: /MODE <channel> <+t / -t>\r\n";
		std::string irssiMessage = ":localhost 472 " + client.getUser().getNickname() + " " + tokens[2] + " :is unknown mode char to me\r\n";
		sendErrorMessage(client, netcatMessage, irssiMessage);
		return;
	}

	const std::vector<int>& members = channel.getMembers();
	for (size_t i = 0; i < members.size(); ++i) {
		int memberSocket = members[i];
		Client& memberClient = _clients[memberSocket];

		if (memberClient.isIrssi)
			fullMessage = topicRightMsgIrssi;
		else
			fullMessage = topicRightMsgNetcat;
		::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
	}
}
