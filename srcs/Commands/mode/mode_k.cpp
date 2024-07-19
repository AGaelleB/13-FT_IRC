#include "../../../includes/Server.hpp"

/* 
 	* This function allows an operator to set or remove a key for a channel. The key can be used to restrict access to the channel.

	— k : Définir/supprimer la clé du canal (mot de passe)

	* /mode <channel> +k <key> - Sets the channel key to <key>.
	* /mode <channel> -k - Removes the channel key.

 */

void Server::modeKCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName) {
	if (tokens[2] == "+k") {
		if (tokens.size() != 4) {
			std::string errorMsgNetcat = std::string(RED) + "Error: Invalid number of parameters for +k mode. Usage: /mode <channel> +k <key>\n" + std::string(RESET);
			std::string errorMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname NOTICE " + client.getUser().getNickname() + " :Invalid number of parameters for +k mode. Usage: /mode <channel> +k <key>\r\n";
			sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
			return;
		}
		if (channel.getChannelKey() != "") {
			std::string errorMsgNetcat = std::string(RED) + "Error: Channel key already set for " + channelName + ". Use /mode <channel> -k to remove the key.\n" + std::string(RESET);
			std::string errorMsgIrssi = ERR_BADCHANNELKEY(client.getUser().getNickname(), channelName);
			sendErrorMessage(client, errorMsgNetcat, errorMsgIrssi);
			return;
		}
		channel.setChannelKey(tokens[3]);
		std::string modeMsgNetcat = std::string(BOLD) + "Channel key for " + channelName + " set to: " + tokens[3] + "\n" + std::string(RESET);
		std::string modeMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " +k " + tokens[3] + "\r\n";
		sendErrorMessage(client, modeMsgNetcat, modeMsgIrssi);
	}
	else if (tokens[2] == "-k") {
		channel.setChannelKey("");
		std::string modeMsgNetcat = std::string(BOLD) + "Channel key for " + channelName + " removed\n" + std::string(RESET);
		std::string modeMsgIrssi = ":" + client.getUser().getNickname() + "!" + client.getUser().getUsername() + "@hostname MODE " + channelName + " -k\r\n";
		sendErrorMessage(client, modeMsgNetcat, modeMsgIrssi);
	}
	else if (client.isIrssi && tokens[2] == "") {
		std::string errorMsgIrssi = ERR_BADCHANNELKEY(client.getUser().getNickname(), channelName);
		sendErrorMessage(client, "", errorMsgIrssi);
	}
}
