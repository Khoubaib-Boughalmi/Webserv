CC= c++ 

NAME= webserv

CFLAGS= -std=c++98 -Wall -Wextra -Werror

SRCS = 	./main.cpp \
		./server/server.cpp \
		./server/ConfigFIleParser.cpp \
		./server/ParserUtils.cpp

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
