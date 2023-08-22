CC = gcc
OPT = -O0
CFLAGS = -std=c99 -fPIC -g -Wall -Wextra -pedantic $(OPT)
#CFLAGS = -std=gnu99 -fPIC -g -Wall -Wextra -pedantic $(OPT) -fsanitize=address -fno-omit-frame-pointer

INCLUDES= -I ./include
LIBRARIES= -lm

CFILES_DIR := ./src
BUILD_DIR := ./build

CFILES := $(wildcard ./src/*.c)
OBJECTS := $(CFILES:$(CFILES_DIR)/%.c=$(BUILD_DIR)/%.o)

EXEC := ./logimin

${EXEC}: ${OBJECTS}
	gcc ${CFLAGS} ${INCLUDES} $^ -o $@ ${LIBRARIES}

${BUILD_DIR}/%.o: ${CFILES_DIR}/%.c
	mkdir -p $(dir $@)
	$(CC) ${CFLAGS} ${INCLUDES}  -c $< -o $@ ${LIBRARIES}

clean:
	rm -rf ${OBJECTS}
	rm -rf ${EXEC}

run: ${EXEC}
	${EXEC}

debug: ${EXEC}
	cgdb ${EXEC}

testleak: ${EXEC}
	valgrind --leak-check=full --show-leak-kinds=all ./$^

profile: ${EXEC}
	mkdir -p ./prof
	rm -rf ./prof/*
	valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes ${EXEC} ${ARGS}
	mv ./callgrind.* ./prof

