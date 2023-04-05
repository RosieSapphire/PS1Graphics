CC=cc
CFLAGS=-Wall -Wextra -Og -g3
INC=-Iinclude -Ideps/rmath/include
LIB=$(shell pkg-config --libs glfw3) -lm -Ldeps/rmath -lrmath
SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c, %.o, $(SRC))
BIN=ps1_graphics

default: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(INC) $(LIB)
	rm -f *.o

%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ $(INC)

clean:
	rm -f $(BIN)
