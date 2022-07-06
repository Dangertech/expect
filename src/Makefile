BINNAME = expect
BUILDDIR = build

SRC = main.cpp frama.cpp
OBJ = ${BUILDDIR}/main.o ${BUILDDIR}/frama.o
FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

CC = g++

default: compile link
debug: compiledbg link

compiledbg: ${SRC}
	mkdir -p ${BUILDDIR}
	${CC} -c ${SRC} -ggdb
	mv *.o ${BUILDDIR}

compile: ${SRC}
	mkdir -p ${BUILDDIR}
	${CC} -c ${SRC}
	mv *.o ${BUILDDIR}

link: ${OBJ}
	${CC} -o ${BINNAME} ${OBJ} ${FLAGS}
