# ------------------------------------- VARS ----------------------------------

NAME		=	webserv

SRC			=	src/main.cpp\
				src/Server/Server.cpp src/Server/ServerManagment.cpp src/Server/ServerCGI.cpp\
				src/Client/Client.cpp\
				src/ServerConfig/ServerConfig.cpp src/ServerConfig/ServerConfigParsing.cpp\
				src/Location/Location.cpp src/Location/createLocation.cpp\
				src/Request/Request.cpp src/Request/RequestParse.cpp src/Request/RequestParseMP.cpp\
				src/Response/Response.cpp src/Response/ResponseConfig.cpp src/Response/ResponseBuild.cpp\

HEADER		=	-I ./include

OBJS		=	$(SRC:.cpp=.o)

CC		=	c++

FLAGS		=	-Wall -Wextra -Werror -std=c++98

EXE_NAME	=	-o $(NAME)

EXEC		=	$(NAME)

# ------------------------------------- RULES ---------------------------------

all: $(NAME)

.cpp.o:
	$(CC) $(FLAGS) $(HEADER) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(HEADER) $(OBJS) $(EXE_NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(EXEC)

re: fclean all

.PHONY: all clean fclean re
