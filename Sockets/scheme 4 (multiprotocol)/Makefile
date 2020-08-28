SRC_DIR = ./src
_SRC := server.c tcp_client.c udp_client.c
SRC := $(patsubst %,$(SRC_DIR)/%,$(_SRC))

OBJ_DIR = ./obj
_OBJ := $(_SRC:.c=.o)
OBJ := $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

SERVER := server
TCP_CLIENT := tcp_client
UDP_CLIENT := udp_client

CC := gcc
CFLAGS := -g
LDFLAGS := -lpthread

.PHONY: all clean

all: $(SERVER) $(TCP_CLIENT) $(UDP_CLIENT)

clean:
		rm -rf $(OBJ_DIR) $(SERVER) $(TCP_CLIENT) $(UDP_CLIENT)

$(SERVER): $(word 1, $(OBJ))
		$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)

$(TCP_CLIENT): $(word 2, $(OBJ))
		$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)

$(UDP_CLIENT): $(word 3, $(OBJ))
		$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)

$(word 1, $(OBJ)): $(word 1, $(SRC))
		$(CC) -c $<
		if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi
		mv $(word 1, $(_OBJ)) $(OBJ_DIR)

$(word 2, $(OBJ)): $(word 2, $(SRC))
		$(CC) -c $<
		if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi
		mv $(word 2, $(_OBJ)) $(OBJ_DIR)

$(word 3, $(OBJ)): $(word 3, $(SRC))
		$(CC) -c $<
		if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi
		mv $(word 3, $(_OBJ)) $(OBJ_DIR)
