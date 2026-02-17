#Directories
SRC_DIR = src
INC_DIR = include
ASSETS_DIR = assets
BUILD_DIR = build

#Libraries
LIBS = -lncurses

#compiler and flags
CC = g++
CFLAGS = -Wall -g -MMD -I${INC_DIR}

#build objects
OBJS = main.o pieces.o screen.o game.o config.o coord.o
BUILD_OBJS =  ${addprefix ${BUILD_DIR}/, ${OBJS}}

pacman: ${BUILD_OBJS}
	${CC} ${CFLAGS} ${BUILD_OBJS} ${LIBS} -o $@

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

${BUILD_DIR}/main.o: ${SRC_DIR}/main.cpp | ${BUILD_DIR}
	${CC} ${CFLAGS} -c  ${SRC_DIR}/main.cpp -o $@

${BUILD_DIR}/screen.o: ${SRC_DIR}/screen.cpp | ${BUILD_DIR}
	${CC} ${CFLAGS} -c  ${SRC_DIR}/screen.cpp -o $@

${BUILD_DIR}/pieces.o: ${SRC_DIR}/pieces.cpp | ${BUILD_DIR}
	${CC} ${CFLAGS} -c  ${SRC_DIR}/pieces.cpp -o $@

${BUILD_DIR}/game.o: ${SRC_DIR}/game.cpp | ${BUILD_DIR}
	${CC} ${CFLAGS} -c  ${SRC_DIR}/game.cpp -o $@

${BUILD_DIR}/config.o: ${SRC_DIR}/config.cpp | ${BUILD_DIR}
	${CC} ${CFLAGS} -c  ${SRC_DIR}/config.cpp -o $@

${BUILD_DIR}/coord.o: ${SRC_DIR}/coord.cpp | ${BUILD_DIR}
	${CC} ${CFLAGS} -c  ${SRC_DIR}/coord.cpp -o $@

clean:
	rm -rf ${BUILD_DIR} pacman

-include $(wildcard ${BUILD_DIR}/*.d)
