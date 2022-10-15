BINNAME = expect
BUILDDIR = build

MAIN = main.cpp
SRC = frama.cpp settings.cpp ecs.cpp infra.cpp cli.cpp \
			input.cpp util.cpp commands.cpp fabric.cpp puppeteer.cpp
FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

CC = g++

default: compile link
debug: compiledbg link

compiledbg: clean ${SRC} ${MAIN}
	mkdir -p ${BUILDDIR}
	${CC} -c ${MAIN} ${SRC} -ggdb -g
	mv *.o ${BUILDDIR}

compile: clean ${SRC} ${MAIN}
	mkdir -p ${BUILDDIR}
	${CC} -c ${MAIN} ${SRC}
	mv *.o ${BUILDDIR}

link: 
	${CC} -o ${BINNAME} ${BUILDDIR}/* ${FLAGS}

clean:
	- rm -rf ${BUILDDIR}
	- rm -f ${BINNAME} ${FRAMABIN}
