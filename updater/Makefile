SRC = updater.c
OBJ = ${SRC:.c=.o}
CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os ${CPPFLAGS}
LDFLAGS = -lX11

all: updater

updater: ${OBJ}
	@${CC} ${CFLAGS} -o $@ ${OBJ} ${LDFLAGS}

