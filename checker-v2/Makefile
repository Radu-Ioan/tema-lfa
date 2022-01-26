CC = g++ 
CFLAGS = -Wall -std=c++17 -g
# CFLAGS = -Wall -std=c++17


BIN = states_search
SOURCES = main.cpp synchronize.cpp
OBJ = $(patsubst %.cpp,%.o,$(SOURCES))

build: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

main.o: main.cpp
	$(CC) -o $@ $< $(CFLAGS) -c

synchronize.o:	synchronize.cpp
	$(CC) -o $@ $< $(CFLAGS) -c

debug: $(SOURCES)
	$(CC) -o $(BIN) $^ $(CFLAGS)

run: $(BIN)
	./$< $(problem)

labyrinth: build

clean:
	rm -rf *.o debug $(BIN)
