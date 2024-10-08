#ifndef SENDMSG_HPP
# define SENDMSG_HPP

// clients
# define ERROR_MAX_CLIENT_SERVER RED "Error: maximum number of clients reached\n" RESET
# define ERROR_MAX_CLIENT RED "Error: maximum number of clients reached. Connection refused ❌\n" RESET

// username
# define MSG_USERNAME BOLD "Enter your username: " RESET
# define ERROR_USERNAME RED "Error: Your username must be less than 9 characters and contain only alphanumeric characters\n" RESET
# define ERROR_USERNAME_LENGHT RED "Error: username must contain between 1 and 9 characters\n" RESET

// nickname
# define MSG_NICKNAME BOLD "Enter your nickname: " RESET
# define ERROR_NICKNAME RED "Error: Your nickname must be less than 9 characters and contain only alphanumeric characters\n" RESET
# define ERROR_NICKNAME_LENGHT RED "Error: nickname must contain between 1 and 9 characters\n" RESET
# define ERROR_NEW_NICKNAME RED "Error: must be: /nick <new_nickname>\n" RESET
# define ERROR_NICKNAME_NOT_AVAILABLE RED "Error: Nickname already taken. Please choose another one\n" RESET

// password
# define MSG_PASSWORD BOLD "Enter Server password: " RESET
# define ERROR_PASSWORD_TOO_LONG RED "Error: password is too long\n\n" RESET
# define ERROR_PASSWORD RED "Wrong password\n" RESET


// commands
# define UNKNOWN_CMD "Unknown command\n"
# define ERROR_CMD_LIST RED "Error: Must be: /list channel OR user\n" RESET
# define ERROR_CMD_PRIVMSG RED "Error: Must be: /msg OR /PRIVMSG <target> <message>\n" RESET
# define ERROR_TARGET_NOT_FOUND RED "Error: Target not found\n" RESET
# define ERROR_CMD_QUIT RED "Error: /quit doesn't require any additional arguments\n" RESET
# define ERROR_MSG_TOO_LONG RED "Error: Message is too long, must be less than 150 characters\n" RESET
# define ERROR_MSG_CLIENT_TOO_LONG RED "Error: Command is too long\n" RESET
# define ERROR_CMD_CHANNEL RED "Error: Must be: /channel OR /join <channelName>\n" RESET
# define ERROR_CHANNELNAME RED "Error: Your channelName must be between 1 and 32 characters and contain only alphanumeric characters\n" RESET
# define ERROR_CHANNEL_ALREADY_EXIST RED "ERROR: Channel already exists\n" RESET
# define ERROR_CHANNEL_FAILED_CREATE RED "ERROR: Failed to create the channel\n" RESET
# define ERROR_CHANNEL_NOT_FOUND RED "ERROR: Channel not found\n" RESET
# define ERROR_CMD_LEAVE RED "Error: Must be: [/leave OR /part] + <channelName>\n" RESET
# define ERROR_CHANNEL_FAILED_LEAVE RED "ERROR: Failled to leave the channel\n" RESET
# define ERROR_NOT_IN_CHANNEL RED "ERROR: Failled to send message, the client must join a channel\n" RESET
# define ERROR_NOT_IN_CHANNEL_TOPIC RED "ERROR: Failled to set Topic, you must be in the channel\n" RESET
# define ERROR_WRONG_CHANNEL_PASS RED "ERROR: Failled to join the channel, password is incorrect\n" RESET
# define ERROR_NO_PASS_CHANNEL RED "ERROR: Failled to join the channel, no password required\n" RESET

#define ERROR_CMD_PRIVMSG_NETCAT RED "Error: Must be: /msg OR /PRIVMSG <target> <message>\n" RESET
#define UNKNOWN_CMD_NETCAT "Unknown command\n"
#define ERROR_CMD_PRIVMSG_IRSSI ":localhost NOTICE * :Must be: /msg OR /PRIVMSG <target> <message>\r\n"
#define UNKNOWN_CMD_IRSSI ":localhost NOTICE * :Unknown command\r\n"


//topic
# define ERROR_TOPIC_SIZE RED "Error: Topic must contain between 1 and 307 characters\n" RESET

//mode
# define ERROR_CMD_MODE RED "Error: Must be: /MODE <channel> <option>\n" RESET
# define ERROR_CMD_MODE_K RED "Error: Must be: /MODE <channel> <+k> <password>\n" RESET
# define ERROR_MODE_NOT_FOUND RED "Error: MODE not found\n" RESET
# define ERROR_CMD_MODE_L RED "Error: Must be: /MODE <channel> <+l> <max users>\n" RESET
# define ERROR_CMD_MODE_O RED "Error: Must be: /MODE <channel> <+o / -o> <user nickname>\n" RESET
# define ERROR_CMD_MODE_T RED "Error: Must be: /MODE <channel> <+t / -t>\n" RESET
# define ERROR_CMD_MODE_T_RIGHT RED "Error: You must have permissions to change this topic\n" RESET
# define ERROR_CMD_MODE_I RED "Error: Must be: /MODE <channel> <+i / -i>\n" RESET

// server
# define MSG_WELCOME BLUE "\n ~~~    Welcome on our IRC Server!   ~~~ \n" RESET
# define MSG_HELP_CLIENT BLUE " ~~~ use /help for more informations ~~~ \n\n" RESET
# define MSG_HELP_SERVER BLUE " ~~~ use /HELP for more informations ~~~ \n\n" RESET
# define MSG_DISCONNECT BOLD "\nServer is shutting down, you have been disconnected ❌\n" RESET
# define MSG_DISCONNECT_CLIENT BOLD "\nYou have been well disconnected ❌\n" RESET
# define MSG_SEND_USER BOLD "\n=============== LIST OF USERS ==============\n\n" RESET
# define MSG_SEND_CHANNEL BOLD "\n============= LIST OF CHANNELS =============\n\n" RESET
# define MSG_END_LIST BOLD "\n============================================\n" RESET

# define MSG_JOIN_CHAN BOLD "You are now in the Channel: " RESET
# define MSG_LEFT_CHAN BOLD "You have left the Channel: " RESET
# define MSG_ALREADY_CHAN BOLD "You are already in the Channel: " RESET

// irssi
# define ERROR_ARGS_IRSSI RED "Error: must be /connect <localhost> <port> <password>\n" RESET


#endif // SENDMSG_HPP
