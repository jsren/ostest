CXX ?= g++

FLAGS ?= -DOSTEST_STD_EXCEPTIONS

library:
	$(CXX) -c -Wall -Wextra -fno-sized-deallocation $(FLAGS) -O3 -std=c++11 ostest.cpp -o ostest.o

example: library
	$(CXX) -Wall -Wextra -O3 -std=c++11 ostest.o example.cpp -o example.exe

clean:
	rm ostest.o example.exe

test: library
	$(CXX) -Wall -Wextra -O3 -std=c++11 -I. ostest.o selftest/* -o test.exe
