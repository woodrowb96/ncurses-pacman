#Directories
SRC_DIR = src
INC_DIR = include
ASSETS_DIR = assets

#Libraries
LIBS = -lncurses

#compiler and flags
CC = g++
CFLAGS = -Wall -g -MMD -I${INC_DIR}

pacman: main.o pieces.o screen.o game.o config.o coord.o
	${CC} ${CFLAGS} main.o pieces.o screen.o game.o config.o coord.o ${LIBS} -o $@

main.o: ${SRC_DIR}/main.cpp
	${CC} ${CFLAGS} -c  ${SRC_DIR}/main.cpp

screen.o: ${SRC_DIR}/screen.cpp
	${CC} ${CFLAGS} -c  ${SRC_DIR}/screen.cpp

pieces.o: ${SRC_DIR}/pieces.cpp
	${CC} ${CFLAGS} -c  ${SRC_DIR}/pieces.cpp

game.o: ${SRC_DIR}/game.cpp
	${CC} ${CFLAGS} -c  ${SRC_DIR}/game.cpp

config.o: ${SRC_DIR}/config.cpp
	${CC} ${CFLAGS} -c  ${SRC_DIR}/config.cpp

coord.o: ${SRC_DIR}/coord.cpp
	${CC} ${CFLAGS} -c  ${SRC_DIR}/coord.cpp

clean:
	rm *.o pacman

-include $(wildcard *.d)
