CC = g++
CFLAGS = -Wall -g

LIBS = -lncurses

pacman: main.o pieces.o screen.o game.o config.o coord.o
	${CC} ${CFLAGS} main.o pieces.o screen.o game.o config.o coord.o ${LIBS} -o $@

main.o: main.cpp
	${CC} ${CFLAGS} -c  main.cpp

screen.o: screen.cpp screen.h
	${CC} ${CFLAGS} -c  screen.cpp

pieces.o: pieces.cpp pieces.h config.h
	${CC} ${CFLAGS} -c  pieces.cpp

game.o: game.cpp game.h config.h
	${CC} ${CFLAGS} -c  game.cpp

config.o: config.cpp config.h
	${CC} ${CFLAGS} -c  config.cpp

coord.o: coord.cpp coord.h
	${CC} ${CFLAGS} -c  coord.cpp

clean:
	rm *.o pacman
