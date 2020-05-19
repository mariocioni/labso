CC      =  gcc
CFLAGS  =  -O -Wall -lm
EXEC    =  bin/start.out
SRC := src
OBJ := obj

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

$(EXEC):  $(OBJECTS)
	$(CC)  $(CFLAGS) -o $(EXEC) $(OBJECTS) -lm

all: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -I$(SRC) -c $< -o $@

buildDirs:
	mkdir -p obj bin log
	
clean:
	rm -f obj/*.o bin/*.out
