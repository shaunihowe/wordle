SRC = ./wordle.c
BIN = ./wordle
CC  = cc

all : $(BIN)

clean :
	rm -f $(BIN)

run : $(BIN)
	$(BIN)

$(BIN) : clean
	$(CC) $(SRC) -o $(BIN)

