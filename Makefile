
CC = gcc
CFLAGS = -g -Wall -Wextra
OBJS = main.o logic.o menu_io.o game_io.o main_menu.o minimax.o
OBJS_PATH = bin/main.o bin/logic.o bin/menu_io.o bin/game_io.o\
	    bin/main_menu.o bin/minimax.o

# The command pkg-config gives compilation flags for the listed packages.
GTK_CFLAGS = `pkg-config --cflags gtk+-3.0` -rdynamic

# The command pkg-config gives linking information for the listed packages.
GTK_LIBS = `pkg-config --libs gtk+-3.0`


EXE_NAME = reversi

all = main

main: ${OBJS}
	${CC} ${CFLAGS} ${GTK_CFLAGS} ${OBJS_PATH} -o ${EXE_NAME} ${GTK_LIBS}

main.o: src/main.c
	${CC} ${CFLAGS} ${GTK_CFLAGS} -c src/main.c ${GTK_LIBS}
	mv main.o bin

logic.o: src/logic/logic.c
	${CC} ${CFLAGS} ${GTK_CFLAGS} -c src/logic/logic.c ${GTK_LIBS}
	mv logic.o bin

menu_io.o: src/input_output/menu_io.c
	${CC} ${CFLAGS} ${GTK_CFLAGS} -c src/input_output/menu_io.c ${GTK_LIBS}
	mv menu_io.o bin

game_io.o: src/input_output/game_io.c
	${CC} ${CFLAGS} ${GTK_CFLAGS} -c src/input_output/game_io.c ${GTK_LIBS}
	mv game_io.o bin

main_menu.o: src/main_menu/main_menu.c
	${CC} ${CFLAGS} ${GTK_CFLAGS} -c src/main_menu/main_menu.c ${GTK_LIBS}
	mv main_menu.o bin

minimax.o: src/minimax/minimax.c
	${CC} ${CFLAGS} ${GTK_CFLAGS} -c src/minimax/minimax.c ${GTK_LIBS}
	mv minimax.o bin

clean:
	rm bin/*.o reversi

