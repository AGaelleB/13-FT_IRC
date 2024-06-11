#include "../../includes/Server.hpp"

void Server::pingCmdClient(std::vector<std::string> tokens, Client& client) {

	if (tokens.size() > 1) {
		std::string response = RPL_PONG(client.getUser().getNickname(), tokens[1]);
		client.sendClientMsg(client.getClientSocket(), response.c_str());
	}
}