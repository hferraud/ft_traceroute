#******************** VARIABLES ********************#

#---------- NAMES ----------#

NAME		=		ft_traceroute

DOCKER_NAME =		traceroute

#---------- DIRECTORIES ----------#

SRC_DIR		=		src/
INC_DIR		=		include/
BUILD_DIR	=		.build/

#---------- SOURCES ----------#

SRC_FILES	=		main.c			\
					parser.c		\
					socket.c		\
					icmp.c			\
					traceroute.c	\
					udp.c			\
					print.c			\
					time.c			\


#---------- BUILD ----------#

SRC			=		$(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ			=		$(addprefix $(BUILD_DIR), $(SRC:.c=.o))
DEPS		=		$(addprefix $(BUILD_DIR), $(SRC:.c=.d))

#---------- COMPILATION ----------#

C_FLAGS		=		-Wall -Werror -Wextra -g3

I_FLAGS		=		-I$(INC_DIR)

D_FLAGS		=		-MMD -MP

#---------- COMMANDS ----------#

RM			=		rm -rf
MKDIR		=		mkdir -p

#******************** RULES ********************#

#---------- GENERAL ----------#

.PHONY:				all
all:				$(NAME)

.PHONY:				clean
clean:
					$(RM) $(BUILD_DIR)

.PHONY:				fclean
fclean:				clean
					$(RM) $(NAME) $(TEST) $(TEST_BONUS)

.PHONY:				re
re:					fclean
					$(MAKE)

#---------- DOCKER ----------#

.PHONY:				docker_build
docker_build:
					docker build -t $(DOCKER_NAME) .

.PHONY:				docker_run
docker_run:
					docker run --rm -it -v .:/$(DOCKER_NAME) -it $(DOCKER_NAME)

#---------- EXECUTABLES ----------#

-include			$(DEPS)

$(NAME):			$(OBJ)
					$(CC) $(OBJ) -lm -o $@

#---------- OBJECTS FILES ----------#

$(BUILD_DIR)%.o:	%.c
					$(MKDIR) $(shell dirname $@)
					$(CC) $(C_FLAGS) $(D_FLAGS) $(I_FLAGS) -c $< -o $@
