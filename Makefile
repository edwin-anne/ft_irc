# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: Edwin ANNE <eanne@student.42lehavre.fr>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/06 00:00:00 by Edwin ANNE        #+#    #+#              #
#    Updated: 2025/07/08 10:51:39 by Edwin ANNE       ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Variables
NAME            = ircserv
CC              = c++
CFLAGS          = -Wall -Wextra -Werror -Iincludes -std=c++98
SRCDIR          = src
OBJDIR          = obj
SOURCES         = $(wildcard $(SRCDIR)/**/*.cpp) $(wildcard $(SRCDIR)/*.cpp)
OBJECTS         = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Colors for output
GREEN           = \033[0;32m
RED             = \033[0;31m
BLUE            = \033[0;34m
YELLOW          = \033[0;33m
NC              = \033[0m

# Rules
all: $(NAME)

$(NAME): $(OBJECTS)
	@echo "$(BLUE)Linking $(NAME)...$(NC)"
	@$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully!$(NC)"

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(NC)"
	@rm -rf $(OBJDIR)
	@echo "$(GREEN)✓ Object files cleaned!$(NC)"

fclean: clean
	@echo "$(RED)Cleaning executable...$(NC)"
	@rm -f $(NAME)
	@echo "$(GREEN)✓ Executable cleaned!$(NC)"

re: fclean all

.PHONY: all clean fclean re
