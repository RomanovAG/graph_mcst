CC=g++ -std=c++23
CFLAGS=-c -g -Wall -Wextra -Wpedantic -Werror -O2
LDFLAGS=-fsanitize=address -static-libasan
SRC_DIR=src
OBJ_DIR=obj
SOURCES=$(SRC_DIR)/main.cpp $(SRC_DIR)/graph.cpp
OBJECTS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
EXECUTABLE=a.out

all: build

build: $(OBJ_DIR) $(EXECUTABLE)

clean:
	rm -rf $(OBJ_DIR)

tests: clean build

$(OBJ_DIR):
	mkdir -p obj

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@