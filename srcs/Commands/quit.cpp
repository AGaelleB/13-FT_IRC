#include "../../includes/Server.hpp"


void Server::quitCmdClientReason(Client& client, std::vector<std::string> tokens, std::string old_nickname) {
	std::string reason = "No reason specified";
	if (tokens.size() > 1) 
		reason = joinTokens(tokens, 1);
	std::cout << RED << "\nClient " << old_nickname << " is disconnected! ❌ [socket: " << client.getClientSocket() << "] [" + reason + "]\n" << RESET << std::endl;

}

void Server::quitCmdClient(Client& client, std::vector<std::string> tokens) {
	if (tokens.size() < 1) {
		std::string errorNetcat = std::string(RED) + "Error: Invalid command format for QUIT\n" + std::string(RESET);
		std::string errorIrssi = ":localhost 421 " + client.getUser().getNickname() + " QUIT :Unknown command\r\n";
		sendErrorMessage(client, errorNetcat, errorIrssi);
		return;
	}

	std::string quitMessage;
	if (tokens.size() == 2)
		quitMessage = tokens[1];
	else
		quitMessage = "Client quit";

	partAllChannels(client);

	std::string disconnectMsgNetcat = std::string(BOLD) + "You have been disconnected\n" + std::string(RESET);
	std::string disconnectMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@localhost QUIT :" + quitMessage + "\r\n";
	sendErrorMessage(client, disconnectMsgNetcat, disconnectMsgIrssi);

	usleep(42);
	
	if (client.getClientSocket() != -1)
		close(client.getClientSocket());

	std::string old_nickname = client.getUser().getNickname();
	if (!old_nickname.empty())
		removeNickname(old_nickname);

	quitCmdClientReason(client, tokens, old_nickname);

	for (int i = 0; i < nfds; ++i) {
		if (fds[i].fd == client.getClientSocket()) {
			fds[i] = fds[nfds - 1];
			nfds--;
			break;
		}
	}
	
	_clients.erase(client.getClientSocket());
}

/* 
	/connect localhost 6667 1
	/list channel
 */