# Compiler
CXX := g++

# Compiler flags
# -std=c++26: Use the C++26 standard
# -g: Generate debugging information
# -O2: Enable optimizations. O1: Minor compiler optimizations | O2: Standard compiler optimizations | O3: Aggressive compiler optimizations
# -Wall: Enable all warnings
CXXFLAGS := -std=c++26 -g -O2 -Wall

# Linker flags and libraries
# Use pkg-config to get the necessary flags for Vulkan and GLFW.
LDFLAGS := $(shell pkg-config --libs vulkan glfw3)
LDFLAGS += -L./src/libs

# Include directories
CPPFLAGS := -I./src/libs

# Project structure
TARGET := bin/vulkan_project
SRC_DIRS := $(shell find ./src -type d)
SRC_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJ_DIR := obj
OBJ_FILES := $(patsubst ./src/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Check if the goal is 'test' to enable quiet compilation.
# If MAKECMDGOALS contains 'test', Q will be set to '@' to silence commands.
Q :=
ifneq (,$(findstring test,$(MAKECMDGOALS)))
  Q := @
endif

# Default target
all: $(TARGET)

# Link the program
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(@D)
	$(Q)$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $@ $(LDFLAGS)

# Compile source files into object files
$(OBJ_DIR)/%.o: ./src/%.cpp
	@mkdir -p $(@D)
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Target to build and run the application
test: $(TARGET)
	@./$(TARGET)

# Clean up build files
clean:
	rm -rf ./bin ./obj

# Phony targets
.PHONY: all clean test
