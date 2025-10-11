CXX   := g++
FLAGS := -std=c++17 -Wall -Iinclude

SRC := src/main.cpp src/scanner.cpp src/parser.cpp src/renamer.cpp src/allocator.cpp
OBJ := $(SRC:src/%.cpp=build/%.o)
TARGET := 412alloc

build: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(FLAGS) $(OBJ) -o $@

build/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET)