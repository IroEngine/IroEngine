# Compiler
CXX := g++
SHADERC := glslc

# Compiler flags
CXXFLAGS := -std=c++23 -g -O2 -Wall

# Linker flags and libraries
LDFLAGS := $(shell pkg-config --libs vulkan glfw3)
LDFLAGS += -L./src/lib/discord -Wl,-rpath,'$$ORIGIN' -ldiscord_partner_sdk -lpthread -ldl

# Check the Operating System
OS := $(shell uname -s)

# Add fontconfig library only if we are on Linux
ifeq ($(OS), Linux)
    LDFLAGS += $(shell pkg-config --libs fontconfig)
endif

# Include directories
CPPFLAGS := -I./src/lib -I./src

# Project structure
TARGET := bin/IroEngine
SRC_DIRS := $(shell find ./src -type d)
SRC_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJ_DIR := obj
OBJ_FILES := $(patsubst ./src/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Shader files
SHADER_SRC_DIR := ./src/shaders
SHADER_SRC_FILES := $(wildcard $(SHADER_SRC_DIR)/*.vert $(SHADER_SRC_DIR)/*.frag)
SHADER_OBJ_FILES := $(patsubst $(SHADER_SRC_DIR)/%.vert,obj/shaders/%.vert.o,$(wildcard $(SHADER_SRC_DIR)/*.vert))
SHADER_OBJ_FILES += $(patsubst $(SHADER_SRC_DIR)/%.frag,obj/shaders/%.frag.o,$(wildcard $(SHADER_SRC_DIR)/*.frag))


Q :=
ifneq (,$(findstring test,$(MAKECMDGOALS)))
  Q := @
endif

# Default target
all: $(TARGET)

# Link the program
$(TARGET): $(OBJ_FILES) $(SHADER_OBJ_FILES)
	@mkdir -p $(@D)
	$(Q)$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compile source files into object files
$(OBJ_DIR)/%.o: ./src/%.cpp
	@mkdir -p $(@D)
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Compile and embed shaders into object files
obj/shaders/%.vert.o: $(SHADER_SRC_DIR)/%.vert
	@mkdir -p $(@D)
	$(eval TMP_SPV := $(shell mktemp))
	$(eval SYMBOL_NAME := $(subst .,_,$(notdir $<)))
	$(Q)$(SHADERC) $< -o $(TMP_SPV)
	$(Q)xxd -i -n spirv_$(SYMBOL_NAME) $(TMP_SPV) | $(CXX) $(CXXFLAGS) $(CPPFLAGS) -x c++ -c - -o $@
	$(Q)rm $(TMP_SPV)

obj/shaders/%.frag.o: $(SHADER_SRC_DIR)/%.frag
	@mkdir -p $(@D)
	$(eval TMP_SPV := $(shell mktemp))
	$(eval SYMBOL_NAME := $(subst .,_,$(notdir $<)))
	$(Q)$(SHADERC) $< -o $(TMP_SPV)
	$(Q)xxd -i -n spirv_$(SYMBOL_NAME) $(TMP_SPV) | $(CXX) $(CXXFLAGS) $(CPPFLAGS) -x c++ -c - -o $@
	$(Q)rm $(TMP_SPV)

# Target to build and run the application
test: all
	@cp src/lib/discord/libdiscord_partner_sdk.so bin/
	@cd bin && ./IroEngine

# Clean up build files
clean:
	@rm -rf ./bin ./obj

# Phony targets
.PHONY: all clean test
