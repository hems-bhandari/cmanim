#!/bin/bash
# Run all example animations

set -e  # Exit on error

cd "$(dirname "$0")/.."

echo "========================================="
echo "Building and running ALL examples"
echo "========================================="
echo ""

# Build library first
echo "Building library..."
make clean
make build/libcmanim.a

# Create bin directory for executables
mkdir -p bin

# List of examples
EXAMPLES=(
    "test_render"
    "graph_demo"
    "simple_sine"
    "parametric_heart"
    "spiral"
    "multiple_waves"
)

EXAMPLE_SOURCES=(
    "tests/test_render.c"
    "examples/graph_demo.c"
    "examples/simple_sine.c"
    "examples/parametric_heart.c"
    "examples/spiral.c"
    "examples/multiple_waves.c"
)

# Compile flags
CC=gcc
CFLAGS="-std=c23 -O3 -DGL_SILENCE_DEPRECATION -Wall -Wextra"
INCLUDES="-Iinclude -Isrc"
LIBS="build/libcmanim.a -L/opt/homebrew/lib -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lm"

# Build and run each example
for i in "${!EXAMPLES[@]}"; do
    NAME="${EXAMPLES[$i]}"
    SOURCE="${EXAMPLE_SOURCES[$i]}"

    echo ""
    echo "========================================="
    echo "Example $((i+1))/${#EXAMPLES[@]}: $NAME"
    echo "========================================="

    # Compile
    echo "Compiling $SOURCE..."
    $CC $CFLAGS $INCLUDES "$SOURCE" $LIBS -o "bin/$NAME"

    # Run
    echo "Running..."
    ./bin/$NAME

    echo "✓ Complete"
done

echo ""
echo "========================================="
echo "ALL EXAMPLES COMPLETE!"
echo "========================================="
echo ""
echo "Generated files:"
ls -lh *.mp4 *.png 2>/dev/null || echo "  (check individual directories)"
echo ""
echo "To view:"
echo "  open test_output.png"
echo "  open graph_demo.mp4"
echo "  open simple_sine.mp4"
echo "  open heart.mp4"
echo "  open spiral.mp4"
echo "  open waves.mp4"
