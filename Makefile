.PHONY: clean

CC = gcc
INC = ./include
SRC = ./src
BUILD = ./build
BIN = ./bin
CFLAGS = -Wall -g -I$(INC)

all: $(BIN) $(BUILD) $(BIN)/daidai

$(BIN)/daidai: $(BUILD)/queue.o $(BUILD)/leitner.o $(SRC)/daidai.c
	$(CC) $(CFLAGS) $(SRC)/daidai.c $(BUILD)/queue.o $(BUILD)/leitner.o -o $(BIN)/daidai

$(BUILD)/leitner.o: $(INC)/leitner.h $(SRC)/leitner.c $(INC)/queue.h
	$(CC) $(CFLAGS) -c $(SRC)/leitner.c -o $(BUILD)/leitner.o

$(BUILD)/queue.o: $(INC)/queue.h $(SRC)/queue.c
	$(CC) $(CFLAGS) -c $(SRC)/queue.c -o $(BUILD)/queue.o

$(BIN):
	mkdir $(BIN)

$(BUILD):
	mkdir $(BUILD)

clean:
	rm -f $(BIN)/daidai
	rm -f $(BUILD)/leitner.o
	rm -f $(BUILD)/queue.o
