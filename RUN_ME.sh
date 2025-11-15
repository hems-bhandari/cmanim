#!/bin/bash

echo "================================================"
echo "  cmanim - Math Animation Library"
echo "  Quick Demo"
echo "================================================"
echo ""
echo "This will:"
echo "  1. Build the library"
echo "  2. Run the test (creates test_output.png)"
echo "  3. Run the demo (creates graph_demo.mp4)"
echo ""
read -p "Press Enter to continue..."

cd "$(dirname "$0")"

echo ""
echo "Building..."
make clean
make all

echo ""
echo "Running test..."
make test

echo ""
echo "Running demo (this will take ~5 seconds)..."
make run-demo

echo ""
echo "================================================"
echo "  COMPLETE!"
echo "================================================"
echo ""
echo "View outputs:"
echo "  open test_output.png"
echo "  open graph_demo.mp4"
echo ""
echo "For more examples, see:"
echo "  - QUICKSTART.md"
echo "  - USAGE_GUIDE.md"
echo "  - ./scripts/run_all_examples.sh"
echo ""
