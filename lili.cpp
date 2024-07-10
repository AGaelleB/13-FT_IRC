void	send_infos(Channel &channel, std::string channel_name, Client &client)
{
	std::string	nick		= client.getNickname();
	std::string username	= client.getUsername();
	std::string	list_of_members = client_list_to_str(channel.client_list);
	std::string to_send;

	for (size_t i = 0; i < channel.client_list.size(); i++)
	{
		to_send.clear();
		if (channel.client_list[i].get_is_irssi() == true)
		{
			to_send = RPL_JOIN(user_id(nick, username), channel_name);
			send(channel.client_list[i].get_client_fd(), to_send.c_str(), to_send.size(), 0);
			to_send = RPL_PRIVMSG(nick, username, channel.get_name(), list_of_members);
			send(channel.client_list[i].get_client_fd(), to_send.c_str(), to_send.size(), 0);
			if (!channel.get_description().empty()) {
				to_send = RPL_TOPIC(client.getNickname(), channel_name, channel.get_description());
				send(channel.client_list[i].get_client_fd(), to_send.c_str(), to_send.size(), 0);
			}
			else {
				to_send = RPL_NOTOPIC(client.getNickname(), channel_name);
				send(channel.client_list[i].get_client_fd(), to_send.c_str(), to_send.size(), 0);
			}
		}
		else
		{
			to_send = "\e[0;36m -!- " + nick + " [" + username + "\e[0m@localhost] has joined " + channel_name + "\r\n";
			send(channel.client_list[i].get_client_fd(), to_send.c_str(), to_send.size(), 0);
			to_send = "\e[0;36m -!- " + list_of_members + "\e[0m\r\n";
			send(channel.client_list[i].get_client_fd(), to_send.c_str(), to_send.size(), 0);
			if (!channel.get_description().empty()) {
				to_send = "[topic : " + channel.get_description() + "]\r\n";
				send(channel.client_list[i].get_client_fd(), to_send.c_str(), to_send.size(), 0);
			}
			else {
				to_send = "[no topic set]\r\n";
				send(channel.client_list[i].get_client_fd(), to_send.c_str(), to_send.size(), 0);
			}
		}	
	}
}