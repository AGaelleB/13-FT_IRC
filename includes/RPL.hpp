#ifndef RPL_HPP
#define RPL_HPP

#include "./Library.hpp"

#define MAX_CLIENT_NB 10
#define SUCCESS 0
#define FAILURE -1
#define BREAK 2
#define CONTINUE 3


// User ID
std::string user_id(const std::string &nickname, const std::string &username);

// Welcome messages
std::string RPL_WELCOME(const std::string &client, const std::string &IPaddress);
std::string RPL_YOURHOST(const std::string &client, const std::string &serv_name);
std::string RPL_CREATED(const std::string &client, const std::string &datetime);
std::string RPL_MYINFO(const std::string &client, const std::string &serv_name);
std::string RPL_ISUPPORT(const std::string &client);

// Pass messages
std::string ERR_ALREADYREGISTERED(const std::string &client);
std::string ERR_PASSWDMISMATCH(const std::string &client);

// Privmsg messages
std::string ERR_NOSUCHNICK(const std::string &nick, const std::string &target);
std::string ERR_NOSUCHCHANNEL(const std::string &chan_name);
std::string RPL_PRIVMSG(const std::string &nick, const std::string &username, const std::string &target, const std::string &message);

// Nick messages
std::string ERR_NONICKNAMEGIVEN(const std::string &client);
std::string ERR_ERRONEUSNICKNAME(const std::string &client, const std::string &nickname);
std::string ERR_NICKNAMEINUSE(const std::string &client, const std::string &nickname);
std::string RPL_NICK(const std::string &oclient, const std::string &uclient, const std::string &client);

std::string ERR_NEEDMOREPARAMS(const std::string &client, const std::string &command);

// Join messages
std::string RPL_JOIN(const std::string &user_id, const std::string &channel);
std::string ERR_BANNEDFROMCHAN(const std::string &client, const std::string &channel);
std::string ERR_BADCHANNELKEY(const std::string &client, const std::string &channel);
std::string ERR_CHANNELISFULL(const std::string &client, const std::string &channel);

// Names messages
std::string RPL_NAMREPLY(const std::string &client, const std::string &channel, const std::string &list_of_nicks);
std::string RPL_ENDOFNAMES(const std::string &client, const std::string &channel);

// Kick messages
std::string ERR_USERNOTINCHANNEL(const std::string &client, const std::string &nickname, const std::string &channel);
std::string RPL_KICK(const std::string &nickname, const std::string &username, const std::string &channel, const std::string &kicked, const std::string &reason);

// Topic messages
std::string RPL_TOPIC(const std::string &client, const std::string &channel, const std::string &topic);
std::string RPL_NOTOPIC(const std::string &client, const std::string &channel);
std::string ERR_NOTONCHANNEL(const std::string &client, const std::string &channel);

// Mode messages
std::string ERR_CHANOPRIVSNEEDED(const std::string &client, const std::string &channel);
std::string ERR_UNKNOWNMODE(const std::string &character);

// Pong messages
std::string RPL_PONG(const std::string &client, const std::string &token);

// Part messages
std::string RPL_PART(const std::string &nickname, const std::string &username, const std::string &channel, const std::string &reason);

// Quit messages
std::string RPL_QUIT(const std::string &user_id, const std::string &reason);
std::string RPL_ERROR(const std::string &user_id, const std::string &reason);

// Utils
std::string ERR_FULL_SERV();
std::string ERR_UNKNOWNCOMMAND(const std::string &client, const std::string &command);

// Mode messages
std::string MODE_USERMSG(const std::string &client, const std::string &mode);
std::string MODE_CHANNELMSG(const std::string &channel, const std::string &mode);
std::string MODE_CHANNELMSGWITHPARAM(const std::string &channel, const std::string &mode, const std::string &param);

// Invite messages
std::string ERR_INVITEONLYCHAN(const std::string &channel);
std::string RPL_INVITING(const std::string &channel, const std::string &nickname);

#endif // RPL_HPP