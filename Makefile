# Makefile for cmanim - C Math Animation Library
# Supports macOS (Apple Silicon & Intel), Linux, and Windows

CC := gcc
AR := ar
CFLAGS := -std=c23 -O3 -Wall -Wextra -Wno-unused-parameter
LDFLAGS :=
LIBS := -lm

# Directories
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN_DIR := bin
TEST_DIR := tests
EXAMPLE_DIR := examples

# Detect platform
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Platform-specific settings
ifeq ($(UNAME_S),Darwin)
    # macOS
    PLATFORM := macos
    HOMEBREW_PREFIX := /opt/homebrew
    ifeq ($(UNAME_M),x86_64)
        HOMEBREW_PREFIX := /usr/local
    endif

    # Check for GLFW via Homebrew
    GLFW_CFLAGS := -I$(HOMEBREW_PREFIX)/include
    GLFW_LIBS := -L$(HOMEBREW_PREFIX)/lib -lglfw

    # Check for FreeType (optional for now)
    FREETYPE_CFLAGS := -I$(HOMEBREW_PREFIX)/include/freetype2
    FREETYPE_LIBS := -L$(HOMEBREW_PREFIX)/lib -lfreetype

    # macOS OpenGL framework
    LDFLAGS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

else ifeq ($(UNAME_S),Linux)
    # Linux
    PLATFORM := linux
    GLFW_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null)
    GLFW_LIBS := $(shell pkg-config --libs glfw3 2>/dev/null)
    FREETYPE_CFLAGS := $(shell pkg-config --cflags freetype2 2>/dev/null)
    FREETYPE_LIBS := $(shell pkg-config --libs freetype2 2>/dev/null)
    LIBS += -ldl -lpthread

else
    # Windows (MinGW)
    PLATFORM := windows
    LIBS += -lopengl32 -lgdi32
endif

# Include paths
INCLUDES := -I$(INC_DIR) -I$(SRC_DIR) $(GLFW_CFLAGS)

# Source files
CORE_SRCS := $(SRC_DIR)/cmanim_core.c \
             $(SRC_DIR)/cmanim_plot.c \
             $(SRC_DIR)/cmanim_renderer.c \
             $(SRC_DIR)/cmanim_render.c \
             $(SRC_DIR)/cmanim_ffmpeg.c \
             $(SRC_DIR)/gl_loader.c

CORE_OBJS := $(CORE_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Library targets
LIB_STATIC := $(BUILD_DIR)/libcmanim.a

# Test and example targets
TEST_RENDER := $(BIN_DIR)/test_render
EXAMPLE_DEMO := $(BIN_DIR)/graph_demo

# Default target
.PHONY: all
all: check-deps $(LIB_STATIC) $(TEST_RENDER)

# Check dependencies
.PHONY: check-deps
check-deps:
	@echo "=== Checking dependencies ==="
	@echo "Platform: $(PLATFORM)"
	@which ffmpeg > /dev/null || (echo "WARNING: ffmpeg not found. Install with: brew install ffmpeg" && false)
ifeq ($(PLATFORM),macos)
	@test -d $(HOMEBREW_PREFIX)/include/GLFW || (echo "ERROR: GLFW not found. Install with: brew install glfw" && false)
	@echo "GLFW: OK ($(HOMEBREW_PREFIX))"
endif
	@echo "Dependencies: OK"
	@echo ""

# Create directories
$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Create static library
$(LIB_STATIC): $(CORE_OBJS)
	@echo "AR $@"
	@$(AR) rcs $@ $^
	@echo "Created static library: $@"

# Test binary
$(TEST_RENDER): $(TEST_DIR)/test_render.c $(LIB_STATIC) | $(BIN_DIR)
	@echo "LD $@"
	@$(CC) $(CFLAGS) $(INCLUDES) $< $(LIB_STATIC) $(LDFLAGS) $(GLFW_LIBS) $(LIBS) -o $@
	@echo "Built test: $@"

# Example demo
$(EXAMPLE_DEMO): $(EXAMPLE_DIR)/graph_demo.c $(LIB_STATIC) | $(BIN_DIR)
	@echo "LD $@"
	@$(CC) $(CFLAGS) $(INCLUDES) $< $(LIB_STATIC) $(LDFLAGS) $(GLFW_LIBS) $(LIBS) -o $@
	@echo "Built example: $@"

# Build and run test
.PHONY: test
test: $(TEST_RENDER)
	@echo ""
	@echo "=== Running test ==="
	@./$(TEST_RENDER)

# Build example demo
.PHONY: demo
demo: $(EXAMPLE_DEMO)

# Run demo
.PHONY: run-demo
run-demo: $(EXAMPLE_DEMO)
	@echo ""
	@echo "=== Running demo ==="
	@./$(EXAMPLE_DEMO)

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@rm -f test_output.png graph_demo.mp4
	@echo "Clean complete"

# Install (optional)
.PHONY: install
install: $(LIB_STATIC)
	@echo "Installing to /usr/local..."
	@mkdir -p /usr/local/lib /usr/local/include
	@cp $(LIB_STATIC) /usr/local/lib/
	@cp $(INC_DIR)/cmanim.h /usr/local/include/
	@echo "Installation complete"

# Print help
.PHONY: help
help:
	@echo "cmanim Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build library and test (default)"
	@echo "  test       - Build and run basic rendering test"
	@echo "  demo       - Build example demo"
	@echo "  run-demo   - Build and run demo (creates graph_demo.mp4)"
	@echo "  clean      - Remove build artifacts"
	@echo "  install    - Install library to /usr/local"
	@echo "  help       - Show this help"
	@echo ""
	@echo "Dependencies (macOS):"
	@echo "  brew install glfw ffmpeg"

# Dependencies tracking
-include $(CORE_OBJS:.o=.d)

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -MM -MT '$(BUILD_DIR)/$*.o' $< > $@
