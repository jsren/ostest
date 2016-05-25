library:
	g++ -c -Wall -Wextra -pedantic -Wno-unknown-pragmas -fno-sized-deallocation -O2 -std=c++17 ostest.cpp -o ostest.o
	
example:
	g++ -Wall -Wextra -pedantic -Wno-unknown-pragmas -fno-sized-deallocation -O2 -std=c++17 ostest.o example.cpp -o example.exe
clean:
	rm ostest.o example.exe
