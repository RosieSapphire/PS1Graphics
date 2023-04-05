CC=cc
CFLAGS=-Wall -Wextra -Og -g3
INC=-Iinclude -Ideps/rmath/include
LIB=$(shell pkg-config --libs glfw3) -lm -Ldeps/rmath -lrmath
SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c, %.o, $(SRC))
BIN=ps1_graphics

default: $(BIN)

$(BIN): $(OBJ)
	@echo Building Ruby Math library...
	@make -C deps/rmath/
	@echo Linking "'"$(BIN)"'"executable...
	@$(CC) $(CFLAGS) $^ -o $@ $(INC) $(LIB)
	@echo Deleting object files...
	@rm -f *.o
	@echo Success!

%.o: src/%.c
	@echo Compiling "'"$^"'"...
	@$(CC) $(CFLAGS) -c $^ $(INC)

clean:
	@rm -f $(BIN) $(OBJ)
	@echo Cleaned up build files.
