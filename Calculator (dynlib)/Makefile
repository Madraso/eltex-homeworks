SRC_DIR = ./src
_SRC := main.c
SRC := $(patsubst %,$(SRC_DIR)/%,$(_SRC))

OBJ_DIR = ./obj
_OBJ := $(_SRC:.c=.o)
OBJ := $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

LIB_SRC_DIR = ./lib/src
_LIB_SRC := add.c diff.c mul.c divide.c
LIB_SRC := $(patsubst %,$(LIB_SRC_DIR)/%,$(_LIB_SRC))

LIB_OBJ_DIR = ./lib/obj
_LIB_OBJ := $(_LIB_SRC:.c=.o)
LIB_OBJ := $(patsubst %,$(LIB_OBJ_DIR)/%,$(_LIB_OBJ))

BIN := calc
LIBS := libadd.so libdiff.so libmul.so libdivide.so

CC := gcc
CFLAGS := -g
LDFLAGS := -ladd -ldiff -lmul -ldivide

.PHONY: all clean

all: $(BIN)

clean:
		rm -rf $(OBJ_DIR) $(LIB_OBJ_DIR) $(BIN) $(LIBS)

$(BIN): $(OBJ) $(LIBS)
		$(CC) $(OBJ) -o $(BIN) $(CFLAGS) -L. $(LDFLAGS)

$(OBJ): $(SRC)
		$(CC) -c $(SRC)
		if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi
		mv $(_OBJ) $(OBJ_DIR)

$(word 1, $(LIBS)): $(word 1, $(LIB_OBJ))
		$(CC) -shared -o $@ $<

$(word 2, $(LIBS)): $(word 2, $(LIB_OBJ))
		$(CC) -shared -o $@ $<

$(word 3, $(LIBS)): $(word 3, $(LIB_OBJ))
		$(CC) -shared -o $@ $<

$(word 4, $(LIBS)): $(word 4, $(LIB_OBJ))
		$(CC) -shared -o $@ $<

$(LIB_OBJ): $(LIB_SRC)
		$(CC) -c -fPIC $(LIB_SRC)
		if [ ! -d "$(LIB_OBJ_DIR)" ]; then mkdir $(LIB_OBJ_DIR); fi
		mv $(_LIB_OBJ) $(LIB_OBJ_DIR)