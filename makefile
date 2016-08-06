library:
	g++ -c -Wall -Wextra -pedantic -Wno-unknown-pragmas -fno-sized-deallocation -DOSTEST_STD_EXCEPTIONS -O2 -std=c++11 ostest.cpp -o ostest.o
	
example: library
	g++ -Wall -Wextra -pedantic -Wno-unknown-pragmas -fno-sized-deallocation -DOSTEST_STD_EXCEPTIONS -static-libstdc++ -O2 -std=c++11 ostest.o example.cpp -o example.exe
	
clean:
	rm ostest.o example.exe
