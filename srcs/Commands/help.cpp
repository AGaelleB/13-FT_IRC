#include "../../includes/Server.hpp"

void Server::helpCmdClient(Client& client) {
	std::string helpMessage = 
		"\nAvailable Commands for clients:\n\n"
		"--------------------------------|----------------------------------------\n"
		"Commands                        | Action\n"
		"--------------------------------|----------------------------------------\n"
		"/help                           | Show the list of available cmds\n"
		"/channel [channel_name]         | Join/create a channel\n"
		"/join [channel_name]            | Join a channel\n"
		"/msg [username] [message]       | Send a private msg to a specific user\n"
		"/quit                           | Disconnect from the server\n"
		"/list user                      | List all available users\n"
		"/list channel                   | List all available channels\n"
		"/nick [new_nickname]            | Change your nickname\n"
		"/part [channel_name]            | Leave a channel\n"
		"/leave [channel_name]           | Leave a channel\n"
		"/topic [channel_name] [topic]   | Set the topic of a channel\n"
		"/kick [channel_name] [user]     | Kick a user from a channel\n"
		"/invite [channel_name] [user]   | Invite a user to a channel\n"
		"/MODE [channel] [mode]          | Change the channel mode:\n"
		"                                |   +i/-i : Set/rm invite-only channel\n"
		"                                |   +t/-t : Set/rm topic restrictions\n"
		"                                |   +k/-k : Set/rm channel key (password)\n"
		"                                |   +o/-o : Give/rm channel operator privilege\n"
		"                                |   +l/-l : Set/rm user limit for the channel\n"
		"                                |   +b/-b : Add/rm banned words\n"
		"--------------------------------|----------------------------------------\n";

	client.sendClientMsg(client.getClientSocket(), helpMessage.c_str());
}
