#include "../../includes/Server.hpp"

/* 

	+i restreint l'acces au salon

	invite permet dajouter un nickname a la liste des users aurosiewrs a rejoindre le salon
	au momment du join check si il fait parti de la liste (isinvite)
	pour invite verifier que le user soit membre du channel sur le quel il invite (ismember)

 */


void Server::inviteCmdClient(Client& client, std::vector<std::string> tokens) {
    if (tokens.size() != 3) {
        std::string errorMsgNetcat = "Error: Invalid number of parameters. Usage: /INVITE <user> <channel>\n";
        std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname ERROR :Invalid number of parameters. Usage: /INVITE <user> <channel>\r\n";
        
        if (client.isIrssi) {
            ::send(client.getClientSocket(), errorMsgIrssi.c_str(), errorMsgIrssi.size(), 0);
        } else {
            ::send(client.getClientSocket(), errorMsgNetcat.c_str(), errorMsgNetcat.size(), 0);
        }
        return;
    }

    int userSocket = -1;
    Client* invitedClient = NULL;
    std::string fullMessage;

    std::string channelName = tokens[2];

    if (channelName[0] != '#')
        channelName = "#" + channelName;

    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it == _channels.end()) {
        std::string errorMsgNetcat = "Error: Channel " + channelName + " not found\n";
        std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname ERROR :Channel " + channelName + " not found\r\n";
        
        if (client.isIrssi) {
            ::send(client.getClientSocket(), errorMsgIrssi.c_str(), errorMsgIrssi.size(), 0);
        } else {
            ::send(client.getClientSocket(), errorMsgNetcat.c_str(), errorMsgNetcat.size(), 0);
        }
        return;
    }

    Channel& channel = it->second;
    if (!channel.isMember(client.getClientSocket())) {
        std::string errorMsgNetcat = "Error: You are not a member of the channel " + channelName + "\n";
        std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname ERROR :You are not a member of the channel " + channelName + "\r\n";
        
        if (client.isIrssi) {
            ::send(client.getClientSocket(), errorMsgIrssi.c_str(), errorMsgIrssi.size(), 0);
        } else {
            ::send(client.getClientSocket(), errorMsgNetcat.c_str(), errorMsgNetcat.size(), 0);
        }
        return;
    }

    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getUser().getNickname() == tokens[1]) {
            userSocket = it->second.getClientSocket();
            invitedClient = &it->second;
            break;
        }
    }

    if (userSocket != -1 && invitedClient != NULL) {
        if (channel.isMember(userSocket)) {
            std::string errorMsgNetcat = "Error: User " + invitedClient->getUser().getNickname() + " is already in the channel " + channelName + "\n";
            std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname ERROR :User " + invitedClient->getUser().getNickname() + " is already in the channel " + channelName + "\r\n";
            
            if (client.isIrssi) {
                ::send(client.getClientSocket(), errorMsgIrssi.c_str(), errorMsgIrssi.size(), 0);
            } else {
                ::send(client.getClientSocket(), errorMsgNetcat.c_str(), errorMsgNetcat.size(), 0);
            }
            return;
        }

        channel.addInvitedMember(userSocket);
        std::string inviteMsgNetcat = client.getUser().getNickname() + " invited " + invitedClient->getUser().getNickname() + " to join " + channelName + "\r\n";
        std::string inviteMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname INVITE " + tokens[1] + " :" + channelName + "\r\n";

        const std::vector<int>& members = channel.getMembers();
        for (size_t i = 0; i < members.size(); ++i) {
            int memberSocket = members[i];
            Client& memberClient = _clients[memberSocket];

            if (memberClient.isIrssi)
                fullMessage = inviteMsgIrssi;
            else
                fullMessage = inviteMsgNetcat;
            ::send(memberSocket, fullMessage.c_str(), fullMessage.size(), 0);
        }
        std::string invitedMessage = "You have been invited to join " + channelName + " by " + client.getUser().getNickname() + "\r\n";
        ::send(userSocket, invitedMessage.c_str(), invitedMessage.size(), 0);
    } else {
        std::string errorMsgNetcat = "Error: User " + tokens[1] + " not found\n";
        std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname ERROR :User " + tokens[1] + " not found\r\n";
        
        if (client.isIrssi) {
            ::send(client.getClientSocket(), errorMsgIrssi.c_str(), errorMsgIrssi.size(), 0);
        } else {
            ::send(client.getClientSocket(), errorMsgNetcat.c_str(), errorMsgNetcat.size(), 0);
        }
        return;
    }
}

void Channel::addInvitedMember(int clientToAdd) {
	if (!isInvitedMember(clientToAdd))
		_membersIsInvite.push_back(clientToAdd);
}

bool Channel::isInvitedMember(int clientSocket) const {
	std::vector<int>::const_iterator it;
	for (it = _membersIsInvite.begin(); it != _membersIsInvite.end(); ++it) {
		if (*it == clientSocket)
			return (true);
	}
	return (false);
}

// /connect localhost 6668 1
