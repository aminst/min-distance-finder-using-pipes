CC = g++ 
CFLAGS = -std=c++11 -Wall -Werror -I includes

all: main-process.out user-handler.out min-distance-handler.out

main-process.out: build/main.o build/utils.o
	$(CC) $(CFLAGS) build/main.o build/utils.o -o main-process.out

user-handler.out: build/user-handler.o build/utils.o
	$(CC) $(CFLAGS) build/user-handler.o build/utils.o -o user-handler.out

min-distance-handler.out: build/min-distance-handler.o build/utils.o
	$(CC) $(CFLAGS) build/min-distance-handler.o build/utils.o -o min-distance-handler.out

build/main.o: src/main.cpp includes/utils.hpp
	$(CC) $(CFLAGS) -c src/main.cpp -o build/main.o

build/utils.o: src/utils.cpp includes/utils.hpp
	$(CC) $(CFLAGS) -c src/utils.cpp -o build/utils.o

build/user-handler.o: src/user-handler.cpp includes/utils.hpp
	$(CC) $(CFLAGS) -c src/user-handler.cpp -o build/user-handler.o

build/min-distance-handler.o: src/min-distance-handler.cpp includes/utils.hpp
	$(CC) $(CFLAGS) -c src/min-distance-handler.cpp -o build/min-distance-handler.o

.PHONY: clean
clean:
	rm build/*.o
	rm *.out