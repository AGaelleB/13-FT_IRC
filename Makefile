
NAME = ircserv

CC = c++

CPP_FLAGS = -Wall -Wextra -Werror -std=c++98 -g3

OBJ_DIR = objs
OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.cpp=.o)))

SRCS =	main.cpp \
		Client.cpp \
		irssi.cpp \
		Parsing.cpp \
		Server.cpp \
		User.cpp \

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

$(OBJ_DIR)/%.o: srcs/%.cpp
	@$(CC) $(CPP_FLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@$(CC) $(CPP_FLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)[Make : '$(NAME)' is done ​​✅​]$(RESET)"

clean :
	@rm -rf $(OBJ_DIR)
	@echo "$(BLUE)[clean : .o of $(NAME) have been deleted ​​🗑️ ​🧹​]$(RESET)"

fclean:
	@$(RM) $(OBJ_DIR)
	@$(RM) $(NAME)
	@echo "$(BLUE)[fclean : '$(NAME)' is done ​​🗑️ ​🧹​]$(RESET)"

re: fclean all

.PHONY: fclean all re
