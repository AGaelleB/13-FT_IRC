NAME = ircserv

CC = c++

CPP_FLAGS = -Wall -Wextra -Werror -std=c++98 -g3

OBJ_DIR = objs

SRCS = srcs/main.cpp \
	srcs/Client/Client.cpp \
	srcs/Client/User.cpp \
	\
	srcs/Commands/AllCommands.cpp \
	srcs/Commands/channel.cpp \
	srcs/Commands/channelMsg.cpp \
	srcs/Commands/help.cpp \
	srcs/Commands/leave.cpp \
	srcs/Commands/list.cpp \
	srcs/Commands/nick.cpp \
	srcs/Commands/Pass.cpp \
	srcs/Commands/ping.cpp \
	srcs/Commands/privMsg.cpp \
	srcs/Commands/quit.cpp \
	srcs/Commands/topic.cpp \
	\
	srcs/Commands/mode/mode_i.cpp \
	srcs/Commands/mode/mode_k.cpp \
	srcs/Commands/mode/mode_l.cpp \
	srcs/Commands/mode/mode_o.cpp \
	srcs/Commands/mode/mode_t.cpp \
	srcs/Commands/mode/mode.cpp \
	\
	srcs/RPL/RPL.cpp \
	\
	srcs/Registration/Authentification.cpp \
	srcs/Registration/Parsing.cpp \
	\
	srcs/Server/irssi.cpp \
	srcs/Server/Server.cpp \
	srcs/Server/Utils.cpp

OBJS = $(SRCS:srcs/%.cpp=$(OBJ_DIR)/%.o)

RM = rm -rf

RED=\033[1;31m
GREEN=\033[1;32m
YELLOW=\033[1;33m
BLUE=\033[1;34m
MAGENTA=\033[1;35m
CYAN=\033[1;36m
RESET=\033[0m

all: banner $(NAME)
	@echo "$(BLUE)██████████████████████ '$(NAME)' is DONE ███████████████████████$(RESET)"
	@echo "       "

head:
	@echo "$(BLUE)█████████████████████ Making ft_irc Server █████████████████████$(RESET)"

# Phonebook making rules
$(NAME): head $(OBJS) $(MAIN_OBJ) $(HEADERS_FILES)
	@$(CC) $(CPP_FLAGS) $(OBJS) -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/Client $(OBJ_DIR)/Commands $(OBJ_DIR)/Registration $(OBJ_DIR)/Server $(OBJ_DIR)/RPL

$(OBJ_DIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	@printf "$(BLUE)█ $(YELLOW)Compiling$(RESET) $<:\r\t\t\t\t\t\t\t..."
	@$(CC) $(CPP_FLAGS) -c $< -o $@
	@echo "\r\t\t\t\t\t\t\t$(GREEN){DONE}$(BLUE) █$(RESET)"

clean:
	@$(RM) $(OBJ_DIR)
	@echo "$(BLUE)[clean : .o of $(NAME) have been deleted 🗑️ 🧹]$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(BLUE)[fclean : '$(NAME)' is done 🗑️ 🧹]$(RESET)"

re: fclean all



banner: 
	@echo "	"
	@echo " 		┏━━━┓┏━━━━┓    ┏━━┓┏━━━┓┏━━━┓"
	@echo " 		┃┏━━┛┃┏┓┏┓┃    ┗┫┣┛┃┏━┓┃┃┏━┓┃"
	@echo " 		┃┗━━┓┗┛┃┃┗┛     ┃┃ ┃┗━┛┃┃┃ ┗┛"
	@echo " 		┃┏━━┛  ┃┃       ┃┃ ┃┏┓┏┛┃┃ ┏┓"
	@echo " 		┃┃    ┏┛┗┓┏━━━┓┏┫┣┓┃┃┃┗┓┃┗━┛┃"
	@echo " 		┗┛    ┗━━┛┗━━━┛┗━━┛┗┛┗━┛┗━━━┛"
	@echo "	"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "━━━━━━━━━━━━┃ Made with love by : $(GREEN) Batt & Gaelle $(RESET) ┃━━━━━━━━━━━━━"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"


.PHONY: fclean all re clean