JFLAGS = -g
JC = javac
JVM = java

all: build

sources = $(wildcard *.java)
classes = $(sources:.java=.class)

build: $(classes)

run: build
	$(JVM) Main $(problem)

labyrinth: build
	$(JVM) Labyrinth

.PHONY: clean
clean :
	rm -f *.class *~

%.class : %.java
	$(JC) $(JFLAGS) $<
