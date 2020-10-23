CC = gcc
CFLAGS = -g -Wall -Wextra
OBJS = main.o logic.o menu_io.o game_io.o main_menu.o minimax.o
OBJS_PATH = bin/main.o bin/logic.o bin/menu_io.o bin/game_io.o\
	    bin/main_menu.o bin/minimax.o

EXE_NAME = reversi

all = main

main: ${OBJS}
	${CC} ${CFLAGS} ${OBJS_PATH} -o ${EXE_NAME}

main.o: src/main.c
	${CC} ${CFLAGS} -c src/main.c
	mv main.o bin

logic.o: src/logic/logic.c
	${CC} ${CFLAGS} -c src/logic/logic.c
	mv logic.o bin

menu_io.o: src/input_output/menu_io.c
	${CC} ${CFLAGS} -c src/input_output/menu_io.c
	mv menu_io.o bin

game_io.o: src/input_output/game_io.c
	${CC} ${CFLAGS} -c src/input_output/game_io.c
	mv game_io.o bin

main_menu.o: src/main_menu/main_menu.c
	${CC} ${CFLAGS} -c src/main_menu/main_menu.c
	mv main_menu.o bin

minimax.o: src/minimax/minimax.c
	${CC} ${CFLAGS} -c src/minimax/minimax.c
	mv minimax.o bin

clean:
	rm bin/*.o reversi
