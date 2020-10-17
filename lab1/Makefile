FLAGS = -std=c++14 -O2 -Wextra -Wall -Werror -Wno-sign-compare -Wno-unused-result -pedantic

all: main.o
	g++ $(FLAGS) main.o -o solution

main: main.cpp Vector.hpp Item.hpp
	g++ $(FLAGS) -c main.cpp

clean:
	rm -rf *.o