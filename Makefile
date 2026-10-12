# Build with plain g++ (or clang++): `make`, `make run`, `make test`.
CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
CPPFLAGS += -Iinclude

LIB_SRC := $(wildcard src/cg/*.cpp) $(wildcard src/scene/*.cpp)
LIB_OBJ := $(LIB_SRC:src/%.cpp=build/obj/%.o)

.PHONY: all run demo test clean
all: build/showroom build/algorithms_demo build/test_algorithms

build/obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

build/showroom: apps/showroom_main.cpp $(LIB_OBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

build/algorithms_demo: apps/algorithms_demo.cpp $(LIB_OBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

build/test_algorithms: tests/test_algorithms.cpp $(LIB_OBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

run: build/showroom
	./build/showroom

demo: build/algorithms_demo
	./build/algorithms_demo

test: build/test_algorithms
	./build/test_algorithms

clean:
	rm -rf build output
