#include "../../../includes/Server.hpp"

void Server::modeICmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
	if (tokens.size() != 3) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE_I);
		return;
	}

	std::string invitationAccessMsgNetcat;
	std::string invitationAccessMsgIrssi;
	std::string fullMessage;

	if (tokens[2] == "+i") {
		std::cout << YELLOW << "MODE +i activated" << RESET << std::endl;
		channel.setInvitationAccess(true);
		invitationAccessMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "mode/" + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "[+i] by " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "\r\n";
		invitationAccessMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " +i\r\n";
	}
	else if (tokens[2] == "-i") {
		std::cout << YELLOW << "MODE -i activated" << RESET << std::endl;
		channel.setInvitationAccess(false);
		invitationAccessMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "mode/" + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "[-i] by " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "\r\n";
		invitationAccessMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " -i\r\n";
	}
	else {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE_I);
		return;
	}

	const std::vector<int>& members = channel.getMembers();
	for (size_t i = 0; i < members.size(); ++i) {
		int memberSocket = members[i];
		Client& memberClient = _clients[memberSocket];

		if (memberClient.isIrssi)
			fullMessage = invitationAccessMsgIrssi;
		else
			fullMessage = invitationAccessMsgNetcat;
		::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
	}
}
