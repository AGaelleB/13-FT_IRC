#include "../../includes/Server.hpp"

void Server::quitMsgCmdClient(Client& client, std::vector<std::string> tokens) {

	if (tokens.size() != 1) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_QUIT);
		return;
	}

	// Quitter tous les canaux
	leaveAllChannels(client);

	client.sendClientMsg(client.getClientSocket(), MSG_DISCONNECT_CLIENT);
	usleep(42);
	
	// Fermer la socket du client
	if (client.getClientSocket() != -1)
		close(client.getClientSocket());

	// Enlever le nickname de la map
	std::string old_nickname = client.getUser().getNickname();
	if (!old_nickname.empty())
		removeNickname(old_nickname);

	std::cout << RED << "\nClient " << old_nickname << " is disconnected! ❌ [socket: " << client.getClientSocket() << "]" << RESET << std::endl;
	std::cout << BOLD << "Total client(s) still online: " << RESET << nfds - 2 << "/" << _MAX_CLIENTS << std::endl;

	// Supprimer le client de la map des clients
	_clients.erase(client.getClientSocket());

	// Retirer le client de la structure pollfd
	for (int i = 0; i < nfds; ++i) {
		if (fds[i].fd == client.getClientSocket()) {
			fds[i] = fds[nfds - 1];
			nfds--;
			break;
		}
	}
}