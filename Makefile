CC = gcc
STD = -std=c99 -D_GNU_SOURCE

DIR_SRC = ./src
DIR_OBJ = ./build/obj
DIR_BIN = ./build/bin

SRC = $(wildcard ${DIR_SRC}/*.c)
OBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = pmon
BIN_TARGET = ${DIR_BIN}/$(TARGET)


${BIN_TARGET}:${OBJ}
	@mkdir -p $(@D)
	$(CC) $(OBJ) -o $@

${DIR_OBJ}/%.o:${DIR_SRC}/%.c
	@mkdir -p $(@D)
	$(CC) -c $(STD) $< -o $@

.PHONY:clean
clean:
	find ${DIR_OBJ} -name *.o |xargs rm -f

.PHONY : install
install:
	cp $(BIN_TARGET) $(TARGET)
