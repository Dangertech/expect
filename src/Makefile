BINNAME = expect
BUILDDIR = build

MAIN = main.cpp
SRC = frama.cpp settings.cpp ecs.cpp
FRAMATESTS = frama_tests.cpp frama.cpp
FRAMABIN = expect-frama_tests
FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

CC = g++

default: compile link
debug: compiledbg link

frama_test: clean ${FRAMATESTS}
	mkdir -p ${BUILDDIR}
	g++ -c ${FRAMATESTS}
	mv *.o ${BUILDDIR}
	g++ -o ${FRAMABIN} ${BUILDDIR}/* ${FLAGS}

compiledbg: clean ${SRC} ${MAIN}
	mkdir -p ${BUILDDIR}
	${CC} -c ${MAIN} ${SRC} -ggdb
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
