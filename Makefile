CC=g++ -std=c++23
CFLAGS=-c -g -Wall -Wextra -Wpedantic -Werror -O3
LDFLAGS=-fsanitize=address -static-libasan
SRC_DIR=src
OBJ_DIR=obj
SOURCES=$(SRC_DIR)/main.cpp $(SRC_DIR)/graph.cpp
OBJECTS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
EXECUTABLE=a.out
TEST_DIR=test_dir

all: build

build: $(OBJ_DIR) $(EXECUTABLE)

clean:
	rm -rf $(OBJ_DIR)

tests: build
	@echo "Running tests..."
	@for input_file in $(TEST_DIR)/*_in.txt; do \
	    base_name=$$(basename $$input_file _in.txt); \
	    output_file="$(TEST_DIR)/$${base_name}_out.txt"; \
	    ref_file="$(TEST_DIR)/$${base_name}_ref.txt"; \
	    start_time=$$(date +%s.%N); \
	    ./$(EXECUTABLE) < $$input_file > $$output_file; \
	    end_time=$$(date +%s.%N); \
	    duration=$$(awk "BEGIN {print $$end_time - $$start_time}"); \
	    if diff -Z $$output_file $$ref_file; then \
	        result="PASS"; \
	    else \
	        result="FAIL"; \
	    fi; \
	    echo "$${base_name}: $$result (Time: $${duration}s)"; \
	done

$(OBJ_DIR):
	mkdir -p obj

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@