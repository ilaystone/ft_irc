NAME= irc
FLAGS= -Wall -Werror -Wextra -std=c++98
OBJ_DIR= obj
SRC= Channel.cpp check_command.cpp CModes.cpp Commands.cpp INVITE_command.cpp JOIN_command.cpp \
	main.cpp Mode_command.cpp msg_parse.cpp OPER_command.cpp PART_command.cpp Server.cpp \
	TOPIC_command.cpp UModes.cpp User_authentication.cpp User.cpp Away_command.cpp KICK_command.cpp
OBJ=$(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJ)
	@clang++ -o $(NAME) $(OBJ) $(FLAGS)
	@echo
	@echo "---------- \033[1;32mSuccessfully generated irc\033[0m ----------"
	@echo

$(OBJ): $(OBJ_DIR)/%.o : %.cpp | $(OBJ_DIR)
	@echo Compiling : $< "\x1B[1;32m[OK]\x1B[0m"
	@clang++ -g -c $< -o $@

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)
	@echo
	@echo "---------- \033[1;32mSuccessfully deleted irc\033[0m ----------"
	@echo

re: fclean all