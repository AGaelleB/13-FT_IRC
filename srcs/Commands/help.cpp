#include "../../includes/Server.hpp"

void Server::helpCmdClient(Client& client) {
	std::string helpMessage = 
		"\nAvailable Commands for clients:\n\n"
		"Command                   | Action\n"
		"--------------------------|----------------------------------------\n"
		"/help                     | Show the list of available cmds\n"
		"/channel [channel_name]   | Join/create a channel\n"
		"/msg [username] [message] | Send a private msg to a specific user\n"
		"/quit                     | Disconnect from the server\n"
		"/list user                | List all available users\n"
		"/list channel             | List all available channels\n"
		"/nick [new_nickname]      | Change your nickname\n"
		"/whois [username]         | Show info about a specific user\n\n";
	client.sendClientMsg(client.getClientSocket(), helpMessage.c_str());
}

void Server::helpCmdServer() {
	std::string helpMessage = 
		"Server Commands:\n\n"
		"Command                   | Action\n"
		"--------------------------|----------------------------------------\n"
		"/KICK [username]          | Kick a user from a channel.\n"
		"/INVITE [username]        | Invite a user to a channel.\n"
		"/TOPIC [channel] [topic]  | Change or view the channel topic.\n"
		"/MODE [channel] [mode]    | Change the channel mode:\n"
		"                          |   i : Set/remove invite-only channel\n"
		"                          |   t : Set/remove topic restrictions to channel operators\n"
		"                          |   k : Set/remove channel key (password)\n"
		"                          |   o : Give/remove channel operator privilege\n"
		"                          |   l : Set/remove user limit for the channel\n";
	std::cout << helpMessage << std::endl;

}
