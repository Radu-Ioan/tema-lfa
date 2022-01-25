CC = g++ 
CFLAGS = -Wall -std=c++17 -g
# CFLAGS = -Wall -std=c++17


BIN = main
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

labyrinth:
	$(CC) -o $(BIN) bonus.cpp $(CFLAGS)

clean:
	rm -rf *.o debug $(BIN)
