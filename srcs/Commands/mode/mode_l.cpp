#include "../../../includes/Server.hpp"


std::string ERROR_INVALID_MEMBER_LIMIT = "Invalid member limit. It must be a positive integer.\r\n";

void Server::modeLCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
	if (tokens.size() != 4) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE_L); 
		return;
	}

    std::string fullMessage;

    if (tokens[2] == "+l") {
        int newMaxMembers = atoi(tokens[3].c_str());
        if (newMaxMembers <= 0) {
            client.sendClientMsg(client.getClientSocket(), ERROR_INVALID_MEMBER_LIMIT.c_str());
            return;
        }

        channel.setMaxMembers(newMaxMembers);

        // Conversion de int en string en utilisant stringstream
        std::stringstream ss;
        ss << channel.getMaxMembers();
        std::string maxMembersStr = ss.str();

        std::string maxMembersMsgNetcat = std::string(CYAN_IRSSI) + "-" + std::string(RESET) + "!" + std::string(CYAN_IRSSI) + "- " + std::string(RESET) + "mode/" + std::string(CYAN_IRSSI) + channelName + std::string(RESET) + "[+l " + maxMembersStr + "] by " + std::string(BOLD) + client.getUser().getNickname() + std::string(RESET) + "\r\n";
        std::string maxMembersMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " +l " + maxMembersStr + "\r\n";

        const std::vector<int>& members = channel.getMembers();
        for (size_t i = 0; i < members.size(); ++i) {
            int memberSocket = members[i];
            Client& memberClient = _clients[memberSocket];

            if (memberClient.isIrssi)
                fullMessage = maxMembersMsgIrssi;
            else
                fullMessage = maxMembersMsgNetcat;
            ::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
        }
    }
}
