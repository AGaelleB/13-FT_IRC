#include "../../../includes/Server.hpp"

void	Server::modeKCmd(Client& client, std::vector<std::string> tokens, Channel& channel) {
	if (tokens[2] == "+k") {
		if (tokens.size() != 4) {
			client.sendClientMsg(client.getClientSocket(), ERROR_CMD_MODE_K);
			return;
		}
		channel.setChannelKey(tokens[3]);
	}
	else if (tokens[2] == "-k") {
		channel.setChannelKey("");
	}
}

