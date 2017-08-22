# Makefile - (c) 2017 James Renwick
CXX ?= g++
PROFILE ?= application

# ------------------------------
ifeq ($(PROFILE),application)
include profiles/application.mk
# ------------------------------
else
ifeq ($(PROFILE),bare)
include profiles/bare.mk
# ------------------------------
else
$(error Unknown profile)
endif
endif
# ------------------------------

CFLAGS ?= -Wall -Wextra -O3 -std=c++11

.PHONY: library example clean test all

library:
	$(CXX) -c -fno-sized-deallocation $(CFLAGS) $(PROFILE_CFLAGS) ostest.cpp -o ostest.o

example: library
	$(CXX) -Wall -Wextra -O3 -std=c++11 $(PROFILE_CFLAGS) ostest.o example.cpp -o example.exe

test: library
	$(CXX) -Wall -Wextra -O3 -std=c++11 $(PROFILE_CFLAGS) -I. ostest.o selftest/common.cpp selftest/assertion-test.cpp selftest/metadata-test.cpp -o test.exe

all: example test

clean:
	rm ostest.o example.exe test.exe