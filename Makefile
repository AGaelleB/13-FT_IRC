NAME = ircserv

CC = c++

CPP_FLAGS = -Wall -Wextra -Werror -std=c++98 -g3

OBJ_DIR = objs

SRCS = srcs/main.cpp \
       srcs/Client/Client.cpp \
       srcs/Client/User.cpp \
       \
       srcs/Commands/AllCommands.cpp \
       srcs/Commands/help.cpp \
       srcs/Commands/list.cpp \
       srcs/Commands/nick.cpp \
       srcs/Commands/Pass.cpp \
       srcs/Commands/ping.cpp \
       srcs/Commands/privMsg.cpp \
       srcs/Commands/quit.cpp \
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

all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/Client $(OBJ_DIR)/Commands $(OBJ_DIR)/Registration $(OBJ_DIR)/Server $(OBJ_DIR)/RPL

$(OBJ_DIR)/%.o: srcs/%.cpp
	@$(CC) $(CPP_FLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@$(CC) $(CPP_FLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)[Make : '$(NAME)' is done ✅]$(RESET)"

clean :
	@rm -rf $(OBJ_DIR)
	@echo "$(BLUE)[clean : .o of $(NAME) have been deleted 🗑️ 🧹]$(RESET)"

fclean:
	@$(RM) $(OBJ_DIR)
	@$(RM) $(NAME)
	@echo "$(BLUE)[fclean : '$(NAME)' is done 🗑️ 🧹]$(RESET)"

re: fclean all

.PHONY: fclean all re