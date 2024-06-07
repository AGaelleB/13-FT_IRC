#include "../../includes/RPL.hpp"

std::string user_id(const std::string &nickname, const std::string &username) {
	return ":" + nickname + "!" + username + "@localhost";
}

// Welcome messages
std::string RPL_WELCOME(const std::string &client, const std::string &IPaddress) {
	return ":localhost 001 " + client + " :!!! WELCOME TO THE INTERNET RELAY NETWORK !!! | " + client + ":" + IPaddress + "\r\n";
}

std::string RPL_YOURHOST(const std::string &client, const std::string &serv_name) {
	return ":localhost 002 " + client + " :-Your host is " + serv_name + " (localhost), running version 1.0.1 \r\n";
}

std::string RPL_CREATED(const std::string &client, const std::string &datetime) {
	return "\n:localhost 003 " + client + " :-This server was created " + datetime + "\r\n";
}

std::string RPL_MYINFO(const std::string &client, const std::string &serv_name) {
	return ":localhost 004 " + client + " :-" + serv_name + " 1.0.1 io kost k\r\n";
}

std::string RPL_ISUPPORT(const std::string &client) { // A verifier les aoize + mettre bon msg
	return "\n:localhost 005 " + client + " :-CHANNELLEN=32 NICKLEN=9 TOPICLEN=307 : are supported by this server\r\n";
}

// Pass messages
std::string ERR_ALREADYREGISTERED(const std::string &client) {
	return ":localhost 462 " + client + " :You may not reregister.\r\n";
}

std::string ERR_PASSWDMISMATCH(const std::string &client) {
	return ":localhost 464 " + client + " :Password incorrect.\r\n";
}

// Privmsg messages
std::string ERR_NOSUCHNICK(const std::string &nick, const std::string &target) {
	return ":localhost 401 " + nick + " " + target + " :Nickname does not exist.\r\n";
}

std::string ERR_NOSUCHCHANNEL(const std::string &chan_name) {
	return ":localhost 403 " + chan_name + " :No such channel\r\n";
}

std::string RPL_PRIVMSG(const std::string &nick, const std::string &username, const std::string &target, const std::string &message) {
	return ":" + nick + "!" + username + "@localhost PRIVMSG " + target + " " + message + "\r\n";
}

// Nick messages
std::string ERR_NONICKNAMEGIVEN(const std::string &client) {
	return ":localhost 431 " + client + " :There is no nickname.\r\n";
}

std::string ERR_ERRONEUSNICKNAME(const std::string &client, const std::string &nickname) {
	return ":localhost 432 " + client + " " + nickname + " :Erroneus nickname\r\n";
}

std::string ERR_NICKNAMEINUSE(const std::string &client, const std::string &nickname) {
	return ":localhost 433 " + client + " " + nickname + " :Nickname is already in use.\r\n";
}

std::string RPL_NICK(const std::string &oclient, const std::string &uclient, const std::string &client) {
	return ":" + oclient + "!" + uclient + "@localhost NICK " + client + "\r\n";
}

std::string ERR_NEEDMOREPARAMS(const std::string &client, const std::string &command) {
	return ":localhost 461 " + client + " " + command + " :Not enough parameters.\r\n";
}

// Join messages
std::string RPL_JOIN(const std::string &user_id, const std::string &channel) {
	return user_id + " JOIN :" + channel + "\r\n";
}

std::string ERR_BANNEDFROMCHAN(const std::string &client, const std::string &channel) {
	return "474 " + client + " " + channel + " :Cannot join channel (+b)\r\n";
}

std::string ERR_BADCHANNELKEY(const std::string &client, const std::string &channel) {
	return "475 " + client + " " + channel + " :Cannot join channel (+k)\r\n";
}

std::string ERR_CHANNELISFULL(const std::string &client, const std::string &channel) {
	return "471 " + client + " " + channel + " :Cannot join channel (+l)\r\n";
}

// Names messages
std::string RPL_NAMREPLY(const std::string &client, const std::string &channel, const std::string &list_of_nicks) {
	return ":localhost 353 " + client + " " + channel + " :" + list_of_nicks + "\r\n";
}

std::string RPL_ENDOFNAMES(const std::string &client, const std::string &channel) {
	return ":localhost 366 " + client + " " + channel + " :End of /NAMES list.\r\n";
}

// Kick messages
std::string ERR_USERNOTINCHANNEL(const std::string &client, const std::string &nickname, const std::string &channel) {
	return "441 " + client + " " + nickname + " " + channel + " :They aren't on that channel\r\n";
}

std::string RPL_KICK(const std::string &nickname, const std::string &username, const std::string &channel, const std::string &kicked, const std::string &reason) {
	return ":" + nickname + "!" + username + "@localhost" + " KICK " + channel + " " + kicked + " " + reason + "\r\n";
}

// Topic messages
std::string RPL_TOPIC(const std::string &client, const std::string &channel, const std::string &topic) {
	return ":localhost 332 " + client + " " + channel + " :" + topic + "\r\n";
}

std::string RPL_NOTOPIC(const std::string &client, const std::string &channel) {
	return ":localhost 331 " + client + " " + channel + " :No topic is set\r\n";
}

std::string ERR_NOTONCHANNEL(const std::string &client, const std::string &channel) {
	return ":localhost 442 " + client + " " + channel + " :The user is not on this channel.\r\n";
}

// Mode messages
std::string ERR_CHANOPRIVSNEEDED(const std::string &client, const std::string &channel) {
	return ":localhost 482 " + client + " " + channel + " :You're not channel operator\r\n";
}

std::string ERR_UNKNOWNMODE(const std::string &character) {
	return ":localhost 472 " + character + " :is unknown mode char to me.\r\n";
}

// Pong messages
std::string RPL_PONG(const std::string &client, const std::string &token) {
	return "PONG reply to " + client + " " + token + "\r\n";
}

// Part messages
std::string RPL_PART(const std::string &nickname, const std::string &username, const std::string &channel, const std::string &reason) {
	return ":" + nickname + "!" + username + "@localhost PART " + channel + " " + (reason.empty() ? "." : reason) + "\r\n";
}

// Quit messages
std::string RPL_QUIT(const std::string &user_id, const std::string &reason) {
	return user_id + " QUIT :Quit: " + reason + "\r\n";
}

std::string RPL_ERROR(const std::string &user_id, const std::string &reason) {
	return user_id + " ERROR :" + reason + "\r\n";
}

// Utils
std::string ERR_FULL_SERV() {
	return "[Server] You cannot join, the server is already full";
}

std::string ERR_UNKNOWNCOMMAND(const std::string &client, const std::string &command) {
	return ":localhost 421 " + client + " " + command + " :Unknown command\r\n";
}

// Mode messages
std::string MODE_USERMSG(const std::string &client, const std::string &mode) {
	return ":" + client + " MODE " + client + " :" + mode + "\r\n";
}

std::string MODE_CHANNELMSG(const std::string &channel, const std::string &mode) {
	return ":localhost MODE #" + channel + " " + mode + "\r\n";
}

std::string MODE_CHANNELMSGWITHPARAM(const std::string &channel, const std::string &mode, const std::string &param) {
	return ":localhost MODE #" + channel + " " + mode + " " + param + "\r\n";
}

// Invite messages
std::string ERR_INVITEONLYCHAN(const std::string &channel) {
	return ":localhost 473 " + channel + " :" + channel + "\r\n";
}

std::string RPL_INVITING(const std::string &channel, const std::string &nickname) {
	return channel + " " + nickname + "\r\n";
}