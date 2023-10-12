CC= c++ 
NAME=webserv
CFLAGS= -Wall -Wextra -Werror -fsanitize=address -g

SRCS = 	./main.cpp \
		./server/server.cpp \
		./server/client.cpp \
		./server/request.cpp \
		./server/Servers.cpp \
		./server/ParserUtils.cpp \
		./server/Response.cpp \
		./server/Content.cpp \

%.o:%.cpp
	${CC} ${CFLAGS} -c $^ -o $@

OBJS = ${SRCS:.cpp=.o}

${NAME}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${NAME}

all: ${NAME}

clean:
	rm -rf ${OBJS}

fclean: clean
	rm -rf ${NAME}

re: fclean all

.PHONY: all clean fclean re