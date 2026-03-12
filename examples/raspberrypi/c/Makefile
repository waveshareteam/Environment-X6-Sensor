DIR_BIN = ./bin
DIR_Config = ./lib/Config
DIR_Environment_X6 = ./lib/environment_x6
DIR_Examples = ./examples

OBJ_C = $(wildcard ${DIR_Examples}/*.c ${DIR_Config}/*.c ${DIR_Environment_X6}/*.c)
OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = main
BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = gcc
DEBUG = -g -O0 -Wall
CFLAGS += $(DEBUG)

LIB = -lgpiod -lm

${TARGET}:${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ $(LIB)

${DIR_BIN}/%.o : $(DIR_Examples)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB) -I $(DIR_Config) -I $(DIR_Environment_X6)
    
${DIR_BIN}/%.o : $(DIR_Config)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB)

${DIR_BIN}/%.o : $(DIR_Environment_X6)/%.c	
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB) -I $(DIR_Config)

clean :
	rm $(DIR_BIN)/*.* 
	rm $(TARGET) 
