CC = gcc
INC_DIR = ./include
CFLAGS = -I$(INC_DIR)
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
ASM_DIR = ./asm

main:  $(OBJ_DIR)/main.o $(OBJ_DIR)/fs/fs.o $(OBJ_DIR)/http/http.o $(OBJ_DIR)/hash/hash.o
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/server $(OBJ_DIR)/main.o $(OBJ_DIR)/fs/fs.o $(OBJ_DIR)/http/http.o $(OBJ_DIR)/hash/hash.o

$(OBJ_DIR)/main.o:
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o $(CFLAGS)

$(OBJ_DIR)/fs/fs.o: $(SRC_DIR)/fs/fs.c
	@mkdir -p $(OBJ_DIR)/fs
	$(CC) -c $(SRC_DIR)/fs/fs.c -o $(OBJ_DIR)/fs/fs.o  $(CFLAGS)

$(OBJ_DIR)/http/http.o: $(SRC_DIR)/http/http.c
	@mkdir -p $(OBJ_DIR)/http
	$(CC) -c $(SRC_DIR)/http/http.c -o $(OBJ_DIR)/http/http.o  $(CFLAGS)

$(OBJ_DIR)/hash/hash.o: $(SRC_DIR)/hash/hash.c 
	@mkdir -p $(OBJ_DIR)/hash
	$(CC) -c $(SRC_DIR)/hash/hash.c -o $(OBJ_DIR)/hash/hash.o  $(CFLAGS)

debug: CFLAGS += -g -O0
debug: $(OBJ_DIR)/main.o $(OBJ_DIR)/fs/fs.o $(OBJ_DIR)/http/http.o $(OBJ_DIR)/hash/hash.o 
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/server_debug $(OBJ_DIR)/main.o $(OBJ_DIR)/fs/fs.o $(OBJ_DIR)/http/http.o $(OBJ_DIR)/hash/hash.o

assembly: CFLAGS += -S
assembly:
	@mkdir -p $(ASM_DIR)/fs
	@mkdir -p $(ASM_DIR)/http
	@mkdir -p $(ASM_DIR)/hash
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(ASM_DIR)/main.s
	$(CC) $(CFLAGS) $(SRC_DIR)/fs/fs.c -o $(ASM_DIR)/fs/fs.s
	$(CC) $(CFLAGS) $(SRC_DIR)/http/http.c -o $(ASM_DIR)/http/http.s
	$(CC) $(CFLAGS) $(SRC_DIR)/hash/hash.c -o $(ASM_DIR)/hash/hash.s

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
	rm -rf $(ASM_DIR)

	
