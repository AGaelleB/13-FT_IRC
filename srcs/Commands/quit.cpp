#include "../../includes/Server.hpp"

void Server::quitMsgCmdClient(Client& client, std::vector<std::string> tokens) {

	if (tokens.size() != 1) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_QUIT);
		return;
	}

	client.sendClientMsg(client.getClientSocket(), MSG_DISCONNECT_CLIENT);
	usleep(42);
	
	// fermer la socket du client
	if (client.getClientSocket() != -1)
		close(client.getClientSocket());

	// enlever le nickname de la map
	std::string old_nickname = client.getUser().getNickname();
	if (!old_nickname.empty())
		removeNickname(old_nickname);

	std::cout << RED << "\nClient " << client.getUser().getNickname() << " is disconnected! ❌ [socket: " << client.getClientSocket() << "]" << RESET << std::endl;
	nfds--;
	std::cout << BOLD << "Total client(s) still online: " << RESET << nfds - 1 << "/" << _MAX_CLIENTS << std::endl;
}
