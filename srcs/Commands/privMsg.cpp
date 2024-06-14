#include "../../includes/Server.hpp"

// parcourir les tokens pour identifier si le nmessage est privmsg ou si ca concerne un channel
//ex: <Gaga>: PRIVMSG #random :cc


/* 
je veux adapter ma fonction privMsgCmdClient pour qu elle ait le comportement suivant 
 /msg popo,pipi vous allez bien ?

Gaga msg: PRIVMSG popo :vous allez bien ?

Private message command received

Gaga msg: PRIVMSG pipi :vous allez bien ?

Private message command received


ici je veux faire un split sur la "," pour qu'elle duplique la commande a chaque clients, soit ravoir 2 commandes : PRIVMSG pipi et PRIVMSG popo 
 */


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

		usleep(42);

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


// /connect localhost 6667 1


/* 
	il peut aussi privmsg plusieurs users en même temps
	genre /privmsg user1, user2,#channel coucou


	WARNING
	un truc bizarre avec les privmsg, en gros si au debut irssi essaie d envoyer un msg a client_nc_1 et client_nc_2 en meme temps :
	/msg client_nc_1,client_nc_2 Coucou les amis
	personne ne recoit de message
	par contre si client_nc_1 ou client_nc_2 envoi un msg en mettant irssi en premier destinataire ensuite ca marche :
	/msg irssi,client_nc_1 OK LES AMIS
	en revanche si on met en 1er 
	/msg client_nc_1,irssi OK LES AMIS
	la ca n aurait pas marche  

 */