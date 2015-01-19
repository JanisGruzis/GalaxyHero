FLAGS=-lncurses -O3 -std=c++11
GCC=g++
BIN=bin

all:
	$(GCC) main.cpp $(FLAGS) -o $(BIN)/prod

run:
	./$(BIN)/prod
