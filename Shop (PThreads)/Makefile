SRC_DIR = ./src
_SRC := *.c
SRC := $(patsubst %,$(SRC_DIR)/%,$(_SRC))

OBJ_DIR = ./obj
_OBJ := $(_SRC:.c=.o)
OBJ := $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

BIN := shop_pthreads

CC := gcc
CFLAGS := -g
LDFLAGS := -lpthread

.PHONY: all clean

all: $(BIN)

clean:
		rm -rf $(OBJ_DIR) $(BIN)	

$(BIN): $(OBJ)
		$(CC) $(OBJ) -o $(BIN) $(CFLAGS) $(LDFLAGS)

$(OBJ): $(SRC)
		$(CC) -c $(SRC)
		if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi
		mv $(_OBJ) $(OBJ_DIR)