# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ego <ego@student.42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/10 14:31:08 by victorviter       #+#    #+#              #
#    Updated: 2025/10/17 17:59:27 by ego              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SDIR		=	srcs/
IDIR		=	include/
LDIR		=	lib/
ODIR		=	objs/

SRC			=	main.cpp
SRC			+=	ServerCore.cpp Client.cpp Request.cpp \
				Response.cpp RequestHandler.cpp Resource.cpp \
				utils.cpp Cookie.cpp Config.cpp WebServ.cpp 

OBJS		=	$(addprefix $(ODIR), $(SRC:.cpp=.o))
SRCS		=	$(addprefix $(SDIR), $(SRC))

CC			=	c++
CFLAGS		=	-Wall -Wextra -Werror -std=c++98

IFLAGS		=	-I $(IDIR)

RM			=	rm -rf

NAME		=	webserv

all			:	$(NAME)

bonus		:	$(NAME)

$(NAME)		:	$(ODIR) $(OBJS)
				$(CC) $(CFLAGS) $(IFLAGS) $(OBJS) $(LFLAGS) -o $(NAME)
				make -s header
				printf "$(COLOR_G)[OK] $(NAME) is ready!$(C_RESET)\n" || \
				printf "$(COLOR_R)[KO] Something went wrong.$(C_RESET)\n"

$(ODIR)		:
				mkdir -p $(ODIR)
				printf "Object directory created.\n"

$(ODIR)%.o	:	$(SDIR)%.cpp
				echo "Compiling $<..."
				$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@ || \
				(printf "$(COLOR_R)[KO] Compilation failed.$(C_RESET)\n" && false)

clean		:
				echo "Removing object files..."
				$(RM) $(ODIR)
				printf "$(COLOR_O)[OK] All object files have been removed.$(C_RESET)\n"

fclean		:
				echo "Removing object and binary files..."
				$(RM) $(ODIR)
				$(RM) $(NAME)
				printf "$(COLOR_O)[OK] All binary files have been removed.$(C_RESET)\n"

re			:	fclean all

.PHONY		:	all clean fclean re
.SILENT		:	all bonus $(NAME) $(ODIR) $(OBJS) clean fclean re norm header

COLOR_R		= \033[31m
COLOR_O		= \033[38;5;214m
COLOR_Y		= \033[33m
COLOR_G		= \033[32m
COLOR_B		= \033[34m
COLOR_V		= \033[38;5;93m
COLOR_I		= \033[3m
C_RESET		= \033[0m

header	:
			printf "=====================================================\n"
			printf "$(COLOR_O)__        __   _    ____                   $(C_RESET)\n"
			printf "$(COLOR_R)\ \      / /__| |__/ ___|  ___ _ ____   __ $(C_RESET)\n"
			printf "$(COLOR_G) \ \ /\ / / _ \ '_ \___ \ / _ \ '__\ \ / / $(C_RESET)\n"
			printf "$(COLOR_B)  \ V  V /  __/ |_) |__) |  __/ |   \ V /  $(C_RESET)\n"
			printf "$(COLOR_V)   \_/\_/ \___|_.__/____/ \___|_|    \_/   $(C_RESET)\n"
			printf "$(COLOR_I)          			 by hcavet & vviterbo$(C_RESET)\n"
			printf "=====================================================\n"
			