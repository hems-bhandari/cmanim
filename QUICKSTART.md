# cmanim Quick Start Guide

## 🚀 Run Existing Examples (30 seconds)

```bash
cd /Users/nyuad/Desktop/canim

# Test 1: Basic rendering test (creates PNG)
make test
open test_output.png

# Test 2: Full demo animation (creates MP4)
make run-demo
open graph_demo.mp4

# Run ALL examples at once
./scripts/run_all_examples.sh
```

## 📋 All Available Examples

| Example | Command | Output | What it shows |
|---------|---------|--------|---------------|
| Test render | `make test` | `test_output.png` | Basic sine curve |
| Graph demo | `make run-demo` | `graph_demo.mp4` | Animated sin + Lissajous |
| Simple sine | See below | `simple_sine.mp4` | Just sin(x) |
| Heart shape | See below | `heart.mp4` | Parametric heart animation |
| Spiral | See below | `spiral.mp4` | Spiral drawing |
| Multiple waves | See below | `waves.mp4` | 3 sine waves |

## 🎯 Run Individual Examples

### Example 1: Simple Sine Wave
```bash
gcc -std=c23 -O3 -DGL_SILENCE_DEPRECATION -Iinclude -Isrc \
    examples/simple_sine.c build/libcmanim.a \
    -L/opt/homebrew/lib -lglfw \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -lm -o bin/simple_sine

./bin/simple_sine
open simple_sine.mp4
```

### Example 2: Animated Heart
```bash
gcc -std=c23 -O3 -DGL_SILENCE_DEPRECATION -Iinclude -Isrc \
    examples/parametric_heart.c build/libcmanim.a \
    -L/opt/homebrew/lib -lglfw \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -lm -o bin/heart

./bin/heart
open heart.mp4
```

### Example 3: Spiral
```bash
gcc -std=c23 -O3 -DGL_SILENCE_DEPRECATION -Iinclude -Isrc \
    examples/spiral.c build/libcmanim.a \
    -L/opt/homebrew/lib -lglfw \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -lm -o bin/spiral

./bin/spiral
open spiral.mp4
```

### Example 4: Multiple Waves
```bash
gcc -std=c23 -O3 -DGL_SILENCE_DEPRECATION -Iinclude -Isrc \
    examples/multiple_waves.c build/libcmanim.a \
    -L/opt/homebrew/lib -lglfw \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -lm -o bin/waves

./bin/waves
open waves.mp4
```

## 🎨 Create Your Own Animation

### 1. Create a file: `my_animation.c`

```c
#include "include/cmanim.h"
#include <math.h>

float my_function(float x, void *userdata) {
    return sinf(x) * cosf(x * 0.5f);  // Your math here!
}

int main(void) {
    // Setup
    CMRenderConfig cfg = {1920, 1080, 60, 5.0};  // 5 second video
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 1.5f});

    // Add your function
    cm_scene_add_function(scene, my_function, NULL,
                          -6.0f, 6.0f,  // x range
                          2048,          // samples
                          cm_rgba(255, 100, 200, 255),  // color
                          1.0f);         // line width

    // Render
    cm_scene_render_to_mp4(scene, "my_animation.mp4");
    cm_scene_destroy(scene);
    return 0;
}
```

### 2. Compile

```bash
gcc -std=c23 -O3 -DGL_SILENCE_DEPRECATION -Iinclude -Isrc \
    my_animation.c build/libcmanim.a \
    -L/opt/homebrew/lib -lglfw \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -lm -o my_animation
```

### 3. Run

```bash
./my_animation
open my_animation.mp4
```

## 📐 Common Patterns

### Pattern 1: Change Colors
```c
cm_rgba(255, 0, 0, 255)      // Red
cm_rgba(0, 255, 0, 255)      // Green
cm_rgba(0, 0, 255, 255)      // Blue
cm_rgba(255, 128, 0, 255)    // Orange
cm_rgba(128, 0, 128, 255)    // Purple
```

### Pattern 2: Change Duration
```c
CMRenderConfig cfg = {1920, 1080, 60, 10.0};  // 10 seconds
CMRenderConfig cfg = {1920, 1080, 60, 3.0};   // 3 seconds
```

### Pattern 3: Change Resolution
```c
{1920, 1080, 60, 5.0}  // Full HD
{1280, 720, 60, 5.0}   // HD
{3840, 2160, 30, 5.0}  // 4K
```

### Pattern 4: Change Zoom
```c
cm_scene_set_camera(scene, (CMCamera){0, 0, 0.5f});  // Zoomed out
cm_scene_set_camera(scene, (CMCamera){0, 0, 1.0f});  // Normal
cm_scene_set_camera(scene, (CMCamera){0, 0, 2.0f});  // Zoomed in
```

### Pattern 5: Add Animation
```c
typedef struct { CMCurve *curve; } State;

void animate(CMScene *s, double time, void *ud) {
    State *state = (State*)ud;
    float t = time / 3.0;  // Over 3 seconds
    cm_curve_set_drawn_fraction(state->curve, cm_ease_in_out_quad(t));
}

// In main:
State state;
state.curve = cm_scene_add_function(/* ... */);
cm_scene_set_animate_callback(scene, animate, &state);
```

## 🔧 Compilation Shortcuts

### Create a compile script: `compile.sh`

```bash
#!/bin/bash
gcc -std=c23 -O3 -DGL_SILENCE_DEPRECATION -Iinclude -Isrc \
    "$1" build/libcmanim.a \
    -L/opt/homebrew/lib -lglfw \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -lm -o "${1%.c}"
```

Then use:
```bash
chmod +x compile.sh
./compile.sh my_animation.c
./my_animation
```

## 🎓 Learning Path

1. **Start here**: Run `make test` and `make run-demo`
2. **View source**: Read `examples/graph_demo.c` to understand structure
3. **Simple mod**: Change colors or duration in demo, recompile
4. **New function**: Try `sinf(x*x)` or `cosf(x) + sinf(x*2)`
5. **Parametric**: Create a circle or spiral (see examples)
6. **Animation**: Add `animate` callback to draw curve over time
7. **Multiple**: Plot multiple functions on same graph

## 📚 Full Documentation

- `USAGE_GUIDE.md` - Complete API reference with 10 use cases
- `README.md` - Full project documentation
- `STATUS.md` - Implementation status
- `include/cmanim.h` - API header with comments

## 🐛 Troubleshooting

**"No such file or directory: cmanim.h"**
- Use `-Iinclude` when compiling

**"Undefined symbols for architecture arm64"**
- Make sure you link: `build/libcmanim.a -lglfw -framework OpenGL ...`

**"ffmpeg not found"**
```bash
brew install ffmpeg
```

**Video is all white**
- Check your function range matches camera zoom
- Try adding: `printf("Drawing...")` to debug

**Compile errors**
- Ensure you built the library first: `make build/libcmanim.a`
- Use C23: `-std=c23`

## 💡 Tips

- Start with PNG output (`cm_scene_render_frame_to_png`) for faster testing
- Use fewer samples (512) during development, increase (4096) for final render
- Reduce FPS to 30 or duration to 3s for faster iteration
- Check function for NaN with `printf` before rendering

## 🎬 Next Steps

1. Run the examples to see what's possible
2. Modify an example to make it your own
3. Create a new animation from scratch
4. Share your math visualizations!

Happy animating! 🎉
