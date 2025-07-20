# Default Target OS
TARGET_OS ?= linux

Q :=
ifneq (,$(findstring test,$(MAKECMDGOALS)))
  Q := @
endif

# Project structure
TARGET_DIR := bin/$(TARGET_OS)
OBJ_DIR := obj/$(TARGET_OS)
TARGET := $(TARGET_DIR)/IroEngine

# Source files
SRC_DIRS := $(shell find ./src -type d -not -path "./src/lib*")
SRC_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJ_FILES := $(patsubst ./src/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Icon file
ICON_FILE := icon.png
ICON_OBJ_FILE := $(OBJ_DIR)/icon.o

# Shader files
SHADER_SRC_DIR := ./src/shaders
SHADER_OBJ_FILES := $(patsubst $(SHADER_SRC_DIR)/%.vert,$(OBJ_DIR)/shaders/%.vert.o,$(wildcard $(SHADER_SRC_DIR)/*.vert))
SHADER_OBJ_FILES += $(patsubst $(SHADER_SRC_DIR)/%.frag,$(OBJ_DIR)/shaders/%.frag.o,$(wildcard $(SHADER_SRC_DIR)/*.frag))


# --- Platform-Specific Configuration ---

ifeq ($(TARGET_OS),linux)
    # --- Linux Build ---
    CXX := g++
    SHADERC := glslc
    CXXFLAGS := -std=c++23 -g -O2 -Wall
    LDFLAGS := $(shell pkg-config --libs vulkan glfw3 fontconfig)
    LDFLAGS += -L./lib/linux -Wl,-rpath,'$$ORIGIN' -ldiscord_partner_sdk -lpthread -ldl
    CPPFLAGS := -I./src -I./src/lib/discord -I./src/lib $(shell pkg-config --cflags vulkan glfw3 fontconfig)
    
    TARGET_EXEC := $(TARGET)
	COPY_LIBS = cp ./lib/linux/* $(TARGET_DIR)

else ifeq ($(TARGET_OS),windows)
    # --- Windows Build ---
    CXX := x86_64-w64-mingw32-g++
    SHADERC := glslc
    CXXFLAGS := -std=c++23 -g -O2 -Wall
    LDFLAGS := -L./lib/windows/runtime -L./lib/windows/link -ldiscord_partner_sdk -lglfw3 -lvulkan-1 -lgdi32 -mwindows -static-libgcc -static-libstdc++
    CPPFLAGS := -I./src -I./src/lib/discord -I./src/lib/glfw -I./src/lib -I$(VULKAN_SDK)/Include
    
    TARGET_EXEC := $(TARGET).exe
    COPY_LIBS = cp ./lib/windows/runtime/*.dll $(TARGET_DIR)

endif

# --- Build Rules ---

# Default target
all: $(TARGET_EXEC)

# Target to build for both platforms
release:
	@$(MAKE) -s all TARGET_OS=linux
# 	Need to have a way to test Windows support first. Also Windows support is pain and suffering.
# 	@$(MAKE) -s all TARGET_OS=windows

# Link the program
$(TARGET_EXEC): $(OBJ_FILES) $(SHADER_OBJ_FILES) $(ICON_OBJ_FILE)
	@mkdir -p $(@D)
	$(Q)$(COPY_LIBS)
	$(Q)$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
ifeq ($(TARGET_OS),windows)
	$(Q)$(COPY_LIBS)
endif

# Compile source files into object files
$(OBJ_DIR)/%.o: ./src/%.cpp
	@mkdir -p $(@D)
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Compile and embed the icon into an object file
$(ICON_OBJ_FILE): $(ICON_FILE)
	@mkdir -p $(@D)
	$(Q)xxd -i -n iro_engine_icon_png $< | $(CXX) $(CXXFLAGS) $(CPPFLAGS) -x c++ -c - -o $@

# Compile and embed shaders into object files
$(OBJ_DIR)/shaders/%.vert.o: $(SHADER_SRC_DIR)/%.vert
	@mkdir -p $(@D)
	$(eval TMP_SPV := $(shell mktemp))
	$(eval SYMBOL_NAME := $(subst .,_,$(notdir $<)))
	$(Q)$(SHADERC) $< -o $(TMP_SPV)
	$(Q)xxd -i -n spirv_$(SYMBOL_NAME) $(TMP_SPV) | $(CXX) $(CXXFLAGS) $(CPPFLAGS) -x c++ -c - -o $@
	$(Q)rm $(TMP_SPV)

$(OBJ_DIR)/shaders/%.frag.o: $(SHADER_SRC_DIR)/%.frag
	@mkdir -p $(@D)
	$(eval TMP_SPV := $(shell mktemp))
	$(eval SYMBOL_NAME := $(subst .,_,$(notdir $<)))
	$(Q)$(SHADERC) $< -o $(TMP_SPV)
	$(Q)xxd -i -n spirv_$(SYMBOL_NAME) $(TMP_SPV) | $(CXX) $(CXXFLAGS) $(CPPFLAGS) -x c++ -c - -o $@
	$(Q)rm $(TMP_SPV)


# --- Utility Targets ---

# Default test target
test: test-linux

# Target to build and run the Linux application in 1 command
test-linux:
	@$(MAKE) -s all TARGET_OS=linux
	@cp lib/linux/*.so bin/linux/
	@cd bin/linux && ./IroEngine

# Clean up build files
clean:
	@rm -rf ./bin ./obj

# Phony targets
.PHONY: all clean release test test-linux test-windows
