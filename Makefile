# Compiler
CXX := g++
SHADERC := glslc

# Compiler flags
CXXFLAGS := -std=c++26 -g -O2 -Wall

# Linker flags and libraries
LDFLAGS := $(shell pkg-config --libs vulkan glfw3)
LDFLAGS += -L./src/libs

# Include directories
CPPFLAGS := -I./src/libs -I./src

# Project structure
TARGET := bin/vulkan_project
SRC_DIRS := $(shell find ./src -type d)
SRC_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJ_DIR := obj
OBJ_FILES := $(patsubst ./src/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Shader files
SHADER_SRC_DIR := ./src/shaders
SHADER_BUILD_DIR := ./bin/shaders
SHADER_SRC_FILES := $(wildcard $(SHADER_SRC_DIR)/*.vert $(SHADER_SRC_DIR)/*.frag)
SHADER_OBJ_FILES := $(patsubst $(SHADER_SRC_DIR)/%.vert,$(SHADER_BUILD_DIR)/%.vert.spv,$(wildcard $(SHADER_SRC_DIR)/*.vert))
SHADER_OBJ_FILES += $(patsubst $(SHADER_SRC_DIR)/%.frag,$(SHADER_BUILD_DIR)/%.frag.spv,$(wildcard $(SHADER_SRC_DIR)/*.frag))


Q :=
ifneq (,$(findstring test,$(MAKECMDGOALS)))
  Q := @
endif

# Default target
all: $(TARGET)

# Link the program
$(TARGET): $(OBJ_FILES) $(SHADER_OBJ_FILES)
	@mkdir -p $(@D)
	$(Q)$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $@ $(LDFLAGS)

# Compile source files into object files
$(OBJ_DIR)/%.o: ./src/%.cpp
	@mkdir -p $(@D)
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Compile shaders
$(SHADER_BUILD_DIR)/%.spv: $(SHADER_SRC_DIR)/%
	@mkdir -p $(@D)
	$(Q)$(SHADERC) $< -o $@

# Target to build and run the application
test: clean $(TARGET)
	@./$(TARGET)

# Clean up build files
clean:
	rm -rf ./bin ./obj

# Phony targets
.PHONY: all clean test
