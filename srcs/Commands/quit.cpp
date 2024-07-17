#include "../../includes/Server.hpp"

void Server::quitCmdClient(Client& client, std::vector<std::string> tokens) {

	if (tokens.size() != 1 && tokens.size() != 2) {
		client.sendClientMsg(client.getClientSocket(), ERROR_CMD_QUIT);
		return;
	}

	partAllChannels(client);

	client.sendClientMsg(client.getClientSocket(), MSG_DISCONNECT_CLIENT);
	usleep(42);
	
	if (client.getClientSocket() != -1)
		close(client.getClientSocket());

	std::string old_nickname = client.getUser().getNickname();
	if (!old_nickname.empty())
		removeNickname(old_nickname);

	std::cout << RED << "\nClient " << old_nickname << " is disconnected! ❌ [socket: " << client.getClientSocket() << "]" << RESET << std::endl;
	std::cout << BOLD << "Total client(s) still online: " << RESET << nfds - 2 << "/" << _MAX_CLIENTS << std::endl;

	_clients.erase(client.getClientSocket());

	for (int i = 0; i < nfds; ++i) {
		if (fds[i].fd == client.getClientSocket()) {
			fds[i] = fds[nfds - 1];
			nfds--;
			break;
		}
	}
}

/* 
	/connect localhost 6667 1
	/list channel
 */