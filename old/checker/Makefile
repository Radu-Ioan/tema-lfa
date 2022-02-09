CFLAGS =  -g
CC = g++ 
CFLAGS = -Wall -std=c++17

BIN = main

build: $(BIN)

$(BIN): main.cpp
	$(CC) -o $@ $^ $(CFLAGS)

debug: main.cpp
	$(CC) -o $@ $^ $(CFLAGS)

run: $(BIN)
	./$< $(problem)

labyrinth:
	$(CC) -o $(BIN) bonus.cpp $(CFLAGS)

clean:
	rm -rf *.o debug $(BIN)
