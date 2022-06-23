# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jokang <jokang@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/01 00:54:06 by kanghyki          #+#    #+#              #
#    Updated: 2022/06/23 15:55:57 by jokang           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#----------------[ COLOR ]------------------
COMPILE_COLOR	=	"\033[1;34m"
LIB_COLOR		=	"\033[1;32m"
DEL_COLOR		=	"\033[1;31m"
END				=	"\033[0;0m"
CLEAR			=	"\x1b[1A\x1b[M"
#---------------[ libft ]-------------------
LIBFT			=	$(LIBFT_DIR)libft.a
LIBFT_DIR		=	library/libft/
LIBFT_INC		=	$(LIBFT_DIR)inc
#--------------[ SIGNAL ]-----------------
SIG_DIR			=	src/signal
SIG_SRC			=	signal.c
#--------------[ TOKENIZER ]-----------------
TOKEN_DIR		=	src/parser/tokenizer
TOKEN_SRC		=	tokenizer.c\
					tokenizer_core.c\
					tokenizer_utils.c\
					tokenizer_merge.c\
					tokenizer_merge_utils.c\
					lexer.c\
					lexer_utils.c
#--------------[ PARSER ]----------------
PARSER_DIR		=	src/parser
PARSER_SRC		=	parser.c\
					parser_utils.c\
					parser_free_utils.c\
					parser_additional.c\
					ft_merge_str.c
#--------------[ ENV ]----------------
ENV_DIR			=	src/environment
ENV_SRC			=	env_utils.c\
					execute_env.c
#--------------[ ENV_BST ]----------------
ENVBST_DIR		=	src/environment/environment_bst
ENVBST_SRC		=	env_bst_init.c\
					env_bst_insert.c\
					env_bst_search.c\
					env_bst_delete.c
#--------------[ EXECUTE ]----------------
EXECUTE_DIR		=	src/execute_cmd
EXECUTE_SRC		=	cmd.c\
					builtin.c\
					ft_atoull.c\
					pipe.c\
					redir.c\
					arraylist.c
#--------------[ MINISHELL ]----------------
NAME			=	minishell	
OBJ_DIR			=	objects
SRC_DIR			=	src
# SRC				=	main.c
ifdef TEST
	SRC = test.c
	NAME = testshell
else
	SRC = main.c
endif

OBJS			=	$(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))\
					$(addprefix $(OBJ_DIR)/, $(PARSER_SRC:.c=.o))\
					$(addprefix $(OBJ_DIR)/, $(TOKEN_SRC:.c=.o))\
					$(addprefix $(OBJ_DIR)/, $(ENV_SRC:.c=.o))\
					$(addprefix $(OBJ_DIR)/, $(ENVBST_SRC:.c=.o))\
					$(addprefix $(OBJ_DIR)/, $(SIG_SRC:.c=.o))\
					$(addprefix $(OBJ_DIR)/, $(EXECUTE_SRC:.c=.o))
#-----------------[ CMD ]-------------------
CC				=	gcc
CFLAGS			=	-g # -Wall -Wextra -Werror
CPPFLAGS		=	-I include\
					-I $(EXECUTE_DIR)\
					-I/Users/jokang/.brew/opt/readline/include
LDLIBS			=	-l ft -L $(LIBFT_DIR)\
					-l readline -L/Users/jokang/.brew/opt/readline/lib
AR				=	ar -rcs
RM				=	rm -rf

vpath %.c $(SRC_DIR) $(TOKEN_DIR) $(PARSER_DIR) $(ENV_DIR) $(ENVBST_DIR) $(SIG_DIR) $(EXECUTE_DIR)

all: $(NAME)

$(OBJ_DIR)/%.o: %.c
	@echo $(COMPILE_COLOR) "Compiling...\t$<" $(END)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(NAME): $(LIBFT) $(OBJ_DIR) $(OBJS)
	@echo $(COMPILE_COLOR) "Building...\t$@" $(END)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDLIBS)
	@echo $(LIB_COLOR) "Done" $(END)

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

$(LIBFT):
	@make -C $(LIBFT_DIR)

clean:
	@make clean -C $(LIBFT_DIR)
	@$(RM) $(OBJ_DIR)
	@echo $(DEL_COLOR) "clean" $(END)

fclean:
	@make fclean -C $(LIBFT_DIR)
	@$(RM) $(OBJ_DIR) $(NAME)
	@echo $(DEL_COLOR) "fclean" $(END)

test:
	@make fclean
	@make TEST=1 -j 8

re:
	@make fclean
	@make all

.PHONY: all clean fclean re test
