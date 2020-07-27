SRC_DIR = ./src
_SRC := *.c
SRC := $(patsubst %,$(SRC_DIR)/%,$(_SRC))

OBJ_DIR = ./obj
_OBJ := $(_SRC:.c=.o)
OBJ := $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

BIN := text_editor

CC := gcc
CFLAGS := -g
LDFLAGS := -lncursesw

.PHONY: all clean

all: $(BIN)

clean:
		rm -rf ./obj $(BIN)

$(BIN): $(_OBJ)
		$(CC) $(OBJ) -o $(BIN) $(CFLAGS) $(LDFLAGS)

$(_OBJ): $(SRC)
		$(CC) -c $(SRC)
		if [ ! -d "obj" ]; then mkdir obj; fi
		mv *.o ./obj
