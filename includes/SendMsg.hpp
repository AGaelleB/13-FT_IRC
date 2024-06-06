#ifndef SENDMSG_HPP
# define SENDMSG_HPP

# define ERROR_USERNAME RED "Error: Your username must be less than 15 characters and contain only alphanumeric characters\n" RESET
# define ERROR_NICKNAME RED "Error: Your nickname must be less than 15 characters and contain only alphanumeric characters\n" RESET
# define ERROR_NICKNAME_NOT_AVAILABLE RED "Error: Nickname already taken. Please choose another one\n" RESET
# define ERROR_PASSWORD_TOO_LONG RED "Error: password is too long\n\n" RESET
# define ERROR_PASSWORD RED "Wrong password \n\n" RESET

# define MSG_USERNAME BOLD "Enter your username: " RESET
# define MSG_NICKNAME BOLD "Enter your nickname: " RESET
# define MSG_WELCOME BLUE "\n ~~~    Welcome on our IRC Server!   ~~~ \n" RESET
# define MSG_HELP_CLIENT BLUE " ~~~ use /help for more informations ~~~ \n\n" RESET
# define MSG_HELP_SERVER BLUE " ~~~ use /HELP for more informations ~~~ \n\n" RESET
# define MSG_PASSWORD BOLD "Enter Server password: " RESET
# define MSG_DISCONNECT BOLD "\nServer is shutting down, you have been disconnected ❌" RESET

# define UNKNOWN_CMD "Unknown command ❌\n"

#endif // SENDMSG_HPP
