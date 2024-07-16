#include "../../../includes/Server.hpp"

/* 
    — l : Définir/supprimer la limite d’utilisateurs pour le canal

    /mode <channel> +l <nombre>
 */


void Server::modeLCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
    if (tokens.size() != 4) {
        std::string netcatMessage = "Error: Must be: /MODE <channel> <+l> <max users>\r\n";
        std::string irssiMessage = ":localhost 461 " + client.getUser().getNickname() + " MODE_L :Not enough parameters\r\n";
        sendErrorMessage(client, netcatMessage, irssiMessage);
        return;
    }

    std::string fullMessage;

    if (tokens[2] == "+l") {
        int newMaxMembers = atoi(tokens[3].c_str());
        if (newMaxMembers <= 0) {
            std::string netcatMessage = "Invalid member limit. It must be a positive integer.\r\n";
            std::string irssiMessage = ":localhost 400 " + client.getUser().getNickname() + " :Invalid member limit. It must be a positive integer.\r\n";
            sendErrorMessage(client, netcatMessage, irssiMessage);
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

// /connect localhost 6667 1
