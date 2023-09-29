NAME=server
CC= gcc
CFLAGS= -Wall -Wextra -Werror

SRCS =	webserv.c \

%.o:%.c
	${CC} ${CFLAGS} -c $^ -o $@

OBJS = ${SRCS:.c=.o}

${NAME}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${NAME}

all: ${NAME}

clean:
	rm -rf ${OBJS}

fclean: clean
	rm -rf ${NAME}

re: fclean all

.PHONY: all clean fclean re