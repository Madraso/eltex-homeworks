SRC_DIR = ./src
_SRC := server.c client.c
SRC := $(patsubst %,$(SRC_DIR)/%,$(_SRC))

OBJ_DIR = ./obj
_OBJ := $(_SRC:.c=.o)
OBJ := $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

SERVER := server
CLIENT := client

CC := gcc
CFLAGS := -g
LDFLAGS := -lpthread

.PHONY: all clean

all: $(SERVER) $(CLIENT)

clean:
		rm -rf $(OBJ_DIR) $(SERVER) $(CLIENT)

$(SERVER): $(word 1, $(OBJ))
		$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)

$(CLIENT): $(word 2, $(OBJ))
		$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)

$(word 1, $(OBJ)): $(word 1, $(SRC))
		$(CC) -c $<
		if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi
		mv $(word 1, $(_OBJ)) $(OBJ_DIR)

$(word 2, $(OBJ)): $(word 2, $(SRC))
		$(CC) -c $<
		if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi
		mv $(word 2, $(_OBJ)) $(OBJ_DIR)