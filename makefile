CXX ?= g++

library:
	$(CXX) -c -Wall -Wextra -Wno-unknown-pragmas -fno-sized-deallocation -DOSTEST_STD_EXCEPTIONS -O3 -std=c++11 ostest.cpp -o ostest.o
	
example: library
	$(CXX) -Wall -Wextra -Wno-unknown-pragmas -fno-sized-deallocation -DOSTEST_STD_EXCEPTIONS -O3 -std=c++11 ostest.o example.cpp -o example.exe
	
clean:
	rm ostest.o example.exe
