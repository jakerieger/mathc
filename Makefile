# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic 
LDFLAGS = 
LIBS = 

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include

# Target executable
TARGET = $(BIN_DIR)/mathc

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPENDS = $(OBJECTS:.o=.d)

# Include directories
INCLUDES = -I$(INC_DIR)

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Create directories
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Include dependencies
-include $(DEPENDS)

# Debug build
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: clean all

# Release build
release: CXXFLAGS += -O3 -DNDEBUG
release: clean all

# Clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean debug release

# Run
run: $(TARGET)
	./$(TARGET)
