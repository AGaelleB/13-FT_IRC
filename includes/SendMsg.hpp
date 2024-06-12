#ifndef SENDMSG_HPP
# define SENDMSG_HPP

# define ERROR_MAX_CLIENT_SERVER RED "Error: maximum number of clients reached\n" RESET
# define ERROR_MAX_CLIENT RED "Error: maximum number of clients reached. Connection refused ❌\n" RESET
# define ERROR_USERNAME RED "Error: Your username must be less than 15 characters and contain only alphanumeric characters\n" RESET
# define ERROR_NICKNAME RED "Error: Your nickname must be less than 15 characters and contain only alphanumeric characters\n" RESET
# define ERROR_USERNAME_LENGHT RED "Error: username must contain between 3 and 15 characters\n" RESET
# define ERROR_NICKNAME_LENGHT RED "Error: nickname must contain between 3 and 15 characters\n" RESET
# define ERROR_NEW_NICKNAME RED "Error: must be: /nick [new_nickname]\n" RESET
# define ERROR_NICKNAME_NOT_AVAILABLE RED "Error: Nickname already taken. Please choose another one\n" RESET
# define ERROR_PASSWORD_TOO_LONG RED "Error: password is too long\n\n" RESET
# define ERROR_PASSWORD RED "Wrong password \n\n" RESET
# define ERROR_CMD_LIST RED "Error: must be: /list channel OR user\n" RESET
# define ERROR_CMD_PRIVMSG RED "Error: must be: /msg OR /PRIVMSG <target> <message>\n" RESET
# define ERROR_TARGET_NOT_FOUND RED "Error: Target not found\n" RESET
# define ERROR_CMD_QUIT RED "Error: /quit doesn't require any additional arguments\n" RESET
# define ERROR_MSG_TOO_LONG RED "Error: message is too long, must be less than 150 characters\n" RESET
# define ERROR_MSG_CLIENT_TOO_LONG RED "Error: command is too long\n" RESET

# define MSG_USERNAME BOLD "Enter your username: " RESET
# define MSG_NICKNAME BOLD "Enter your nickname: " RESET
# define MSG_WELCOME BLUE "\n ~~~    Welcome on our IRC Server!   ~~~ \n" RESET
# define MSG_HELP_CLIENT BLUE " ~~~ use /help for more informations ~~~ \n\n" RESET
# define MSG_HELP_SERVER BLUE " ~~~ use /HELP for more informations ~~~ \n\n" RESET
# define MSG_PASSWORD BOLD "Enter Server password: " RESET
# define MSG_DISCONNECT BOLD "\nServer is shutting down, you have been disconnected ❌\n" RESET
# define MSG_DISCONNECT_CLIENT BOLD "\nYou have been well disconnected ❌\n" RESET

# define MSG_SEND_USER BOLD "\n============= LIST OF USERS ============\n\n" RESET
# define MSG_END_LIST BOLD "\n========================================\n" RESET


# define UNKNOWN_CMD "Unknown command ❌\n"

#endif // SENDMSG_HPP
