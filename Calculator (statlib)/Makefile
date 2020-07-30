SRC_DIR = ./src
_SRC := *.c
SRC := $(patsubst %,$(SRC_DIR)/%,$(_SRC))

OBJ_DIR = ./obj
_OBJ := $(_SRC:.c=.o)
OBJ := $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

LIB_SRC_DIR = ./lib/src
_LIB_SRC := *.c
LIB_SRC := $(patsubst %,$(LIB_SRC_DIR)/%,$(_LIB_SRC))

LIB_OBJ_DIR = ./lib/obj
_LIB_OBJ := $(_LIB_SRC:.c=.o)
LIB_OBJ := $(patsubst %,$(LIB_OBJ_DIR)/%,$(_LIB_OBJ))

BIN := calc
LIB := libcalc.a

CC := gcc
CFLAGS := -g
LDFLAGS := -lcalc

.PHONY: all clean

all: $(BIN)

clean:
		rm -rf $(OBJ_DIR) $(LIB_OBJ_DIR) $(BIN) $(LIB)

$(BIN): $(OBJ) $(LIB)
		$(CC) $(OBJ) -o $(BIN) $(CFLAGS) -L. $(LDFLAGS)

$(OBJ): $(SRC)
		$(CC) -c $(SRC)
		if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi
		mv $(_OBJ) $(OBJ_DIR)

$(LIB): $(LIB_OBJ)
		ar rc $(LIB) $(LIB_OBJ)

$(LIB_OBJ): $(LIB_SRC)
		$(CC) -c $(LIB_SRC)
		if [ ! -d "$(LIB_OBJ_DIR)" ]; then mkdir $(LIB_OBJ_DIR); fi
		mv $(_LIB_OBJ) $(LIB_OBJ_DIR)