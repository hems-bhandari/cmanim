# cmanim Usage Guide

Complete guide to running and using cmanim for creating math animations.

---

## Quick Start: Running the Examples

### 1. Run the Basic Test
```bash
cd /Users/nyuad/Desktop/canim
make test
```
**Output**: `test_output.png` - A single frame showing sin(x)

**View it**:
```bash
open test_output.png
```

### 2. Run the Full Demo
```bash
make run-demo
```
**Output**: `graph_demo.mp4` - 10 second animation

**View it**:
```bash
open graph_demo.mp4
# Or use any video player
vlc graph_demo.mp4
```

### 3. Clean and Rebuild Everything
```bash
make clean
make all
```

---

## Use Case 1: Simple Function Plot (Static)

Create a file: `my_plot.c`

```c
#include "include/cmanim.h"
#include <math.h>

// Define your function
float my_function(float x, void *userdata) {
    return sinf(x) * cosf(x * 0.5f);
}

int main(void) {
    // Configure: 5 second video at 1080p60
    CMRenderConfig cfg = {
        .width = 1920,
        .height = 1080,
        .fps = 60,
        .duration = 5.0
    };

    // Create scene
    CMScene *scene = cm_scene_create(&cfg);

    // Set camera (zoom controls how much you see)
    cm_scene_set_camera(scene, (CMCamera){
        .x = 0.0f,      // center x
        .y = 0.0f,      // center y
        .zoom = 1.5f    // higher = zoomed in more
    });

    // Add function: y = sin(x)*cos(x/2) on [-10, 10]
    cm_scene_add_function(
        scene,
        my_function,           // Function pointer
        NULL,                  // User data (optional)
        -10.0f, 10.0f,        // x range
        2048,                  // Number of sample points
        cm_rgba(255, 50, 150, 255),  // Pink color
        1.0f                   // Line width
    );

    // Render to MP4
    cm_scene_render_to_mp4(scene, "my_plot.mp4");

    cm_scene_destroy(scene);
    return 0;
}
```

**Compile and run**:
```bash
gcc -std=c23 -O3 -I. my_plot.c build/libcmanim.a \
    -L/opt/homebrew/lib -lglfw \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -lm -o my_plot

./my_plot
```

---

## Use Case 2: Animated Function Drawing

Animate the curve being drawn over time.

```c
#include "include/cmanim.h"
#include <math.h>

float parabola(float x, void *ud) {
    return 0.2f * x * x - 3.0f;
}

// Animation state
typedef struct {
    CMCurve *curve;
} AnimState;

// Called every frame
void animate(CMScene *s, double time, void *userdata) {
    AnimState *state = (AnimState*)userdata;

    // Draw curve from 0% to 100% over 3 seconds
    float progress = time / 3.0;
    if (progress > 1.0f) progress = 1.0f;

    // Use easing for smooth animation
    float eased = cm_ease_in_out_quad(progress);

    cm_curve_set_drawn_fraction(state->curve, eased);
}

int main(void) {
    CMRenderConfig cfg = {1920, 1080, 60, 3.0};
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 1.0f});

    AnimState state;
    state.curve = cm_scene_add_function(scene, parabola, NULL,
                                        -6, 6, 2048,
                                        cm_rgba(100, 200, 255, 255), 1.0f);

    // Set animation callback
    cm_scene_set_animate_callback(scene, animate, &state);

    cm_scene_render_to_mp4(scene, "animated_parabola.mp4");
    cm_scene_destroy(scene);
    return 0;
}
```

---

## Use Case 3: Multiple Functions

Plot multiple functions on the same graph.

```c
#include "include/cmanim.h"
#include <math.h>

float sin_func(float x, void *ud) { return sinf(x); }
float cos_func(float x, void *ud) { return cosf(x); }
float tan_func(float x, void *ud) { return tanf(x); }

int main(void) {
    CMRenderConfig cfg = {1920, 1080, 60, 5.0};
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 1.2f});

    // Add sin(x) in red
    cm_scene_add_function(scene, sin_func, NULL, -6, 6, 2048,
                          cm_rgba(255, 50, 50, 255), 1.0f);

    // Add cos(x) in green
    cm_scene_add_function(scene, cos_func, NULL, -6, 6, 2048,
                          cm_rgba(50, 255, 50, 255), 1.0f);

    // Add tan(x) in blue (will have discontinuities)
    cm_scene_add_function(scene, tan_func, NULL, -1.5f, 1.5f, 2048,
                          cm_rgba(50, 50, 255, 255), 1.0f);

    cm_scene_render_to_mp4(scene, "trig_functions.mp4");
    cm_scene_destroy(scene);
    return 0;
}
```

---

## Use Case 4: Parametric Curves (Spirals, Shapes)

Create parametric curves like spirals, circles, or complex patterns.

```c
#include "include/cmanim.h"
#include <math.h>

// Spiral
float spiral_x(float t, void *ud) { return t * cosf(t); }
float spiral_y(float t, void *ud) { return t * sinf(t); }

// Circle
float circle_x(float t, void *ud) { return 2.0f * cosf(t); }
float circle_y(float t, void *ud) { return 2.0f * sinf(t); }

// Heart shape
float heart_x(float t, void *ud) {
    return 16 * powf(sinf(t), 3) * 0.2f;
}
float heart_y(float t, void *ud) {
    return (13*cosf(t) - 5*cosf(2*t) - 2*cosf(3*t) - cosf(4*t)) * 0.2f;
}

int main(void) {
    CMRenderConfig cfg = {1920, 1080, 60, 8.0};
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 0.8f});

    // Spiral from t=0 to t=4π
    cm_scene_add_parametric(scene, spiral_x, spiral_y, NULL,
                            0, 4*M_PI, 4096,
                            cm_rgba(255, 100, 100, 255), 1.0f);

    // Circle
    cm_scene_add_parametric(scene, circle_x, circle_y, NULL,
                            0, 2*M_PI, 1024,
                            cm_rgba(100, 255, 100, 255), 1.0f);

    // Heart
    cm_scene_add_parametric(scene, heart_x, heart_y, NULL,
                            0, 2*M_PI, 2048,
                            cm_rgba(255, 50, 150, 255), 1.0f);

    cm_scene_render_to_mp4(scene, "parametric_shapes.mp4");
    cm_scene_destroy(scene);
    return 0;
}
```

---

## Use Case 5: Using User Data

Pass data to your functions for parameterized plotting.

```c
#include "include/cmanim.h"
#include <math.h>

// User data structure
typedef struct {
    float amplitude;
    float frequency;
    float phase;
} WaveParams;

// Function that uses user data
float wave_function(float x, void *userdata) {
    WaveParams *params = (WaveParams*)userdata;
    return params->amplitude * sinf(params->frequency * x + params->phase);
}

int main(void) {
    CMRenderConfig cfg = {1920, 1080, 60, 5.0};
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 1.5f});

    // Create different waves with different parameters
    WaveParams wave1 = {1.0f, 1.0f, 0.0f};
    WaveParams wave2 = {0.7f, 2.0f, M_PI/4};
    WaveParams wave3 = {0.5f, 3.0f, M_PI/2};

    cm_scene_add_function(scene, wave_function, &wave1, -6, 6, 2048,
                          cm_rgba(255, 50, 50, 255), 1.0f);

    cm_scene_add_function(scene, wave_function, &wave2, -6, 6, 2048,
                          cm_rgba(50, 255, 50, 255), 1.0f);

    cm_scene_add_function(scene, wave_function, &wave3, -6, 6, 2048,
                          cm_rgba(50, 50, 255, 255), 1.0f);

    cm_scene_render_to_mp4(scene, "wave_superposition.mp4");
    cm_scene_destroy(scene);
    return 0;
}
```

---

## Use Case 6: Complex Animation Sequences

Create multi-stage animations with different phases.

```c
#include "include/cmanim.h"
#include <math.h>

float func1(float x, void *ud) { return sinf(x); }
float func2(float x, void *ud) { return x * x * 0.1f - 2.0f; }

typedef struct {
    CMCurve *curve1;
    CMCurve *curve2;
} AnimState;

void multi_stage_animate(CMScene *s, double time, void *userdata) {
    AnimState *state = (AnimState*)userdata;

    // Stage 1 (0-2s): Draw first curve
    if (time < 2.0) {
        float t = time / 2.0;
        cm_curve_set_drawn_fraction(state->curve1, cm_ease_out_quad(t));
        cm_curve_set_drawn_fraction(state->curve2, 0.0f);
    }
    // Stage 2 (2-3s): Hold first curve
    else if (time < 3.0) {
        cm_curve_set_drawn_fraction(state->curve1, 1.0f);
        cm_curve_set_drawn_fraction(state->curve2, 0.0f);
    }
    // Stage 3 (3-5s): Draw second curve
    else if (time < 5.0) {
        cm_curve_set_drawn_fraction(state->curve1, 1.0f);
        float t = (time - 3.0) / 2.0;
        cm_curve_set_drawn_fraction(state->curve2, cm_ease_in_quad(t));
    }
    // Stage 4 (5-6s): Fade out first curve (change color alpha)
    else {
        cm_curve_set_drawn_fraction(state->curve1, 1.0f);
        cm_curve_set_drawn_fraction(state->curve2, 1.0f);
        // Note: alpha fading not implemented yet, but you could
        // change curve style here
    }
}

int main(void) {
    CMRenderConfig cfg = {1920, 1080, 60, 6.0};
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 1.5f});

    AnimState state;
    state.curve1 = cm_scene_add_function(scene, func1, NULL,
                                         -6, 6, 2048,
                                         cm_rgba(255, 100, 100, 255), 1.0f);
    state.curve2 = cm_scene_add_function(scene, func2, NULL,
                                         -6, 6, 2048,
                                         cm_rgba(100, 255, 100, 255), 1.0f);

    cm_scene_set_animate_callback(scene, multi_stage_animate, &state);
    cm_scene_render_to_mp4(scene, "multi_stage.mp4");
    cm_scene_destroy(scene);
    return 0;
}
```

---

## Use Case 7: Render Single Frame to PNG (Debugging)

Useful for debugging or creating static images.

```c
#include "include/cmanim.h"
#include <math.h>

float func(float x, void *ud) { return sinf(x * x); }

int main(void) {
    CMRenderConfig cfg = {1920, 1080, 60, 1.0};
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 1.5f});

    cm_scene_add_function(scene, func, NULL, -3, 3, 4096,
                          cm_rgba(200, 100, 255, 255), 1.0f);

    // Render just frame 0 to PNG
    cm_scene_render_frame_to_png(scene, 0, "static_plot.png");

    cm_scene_destroy(scene);
    return 0;
}
```

---

## Use Case 8: Different Resolutions & Frame Rates

```c
#include "include/cmanim.h"
#include <math.h>

float func(float x, void *ud) { return cosf(x); }

int main(void) {
    // 4K @ 30fps for high quality
    CMRenderConfig cfg_4k = {3840, 2160, 30, 5.0};

    // 720p @ 60fps for smooth playback
    CMRenderConfig cfg_720p = {1280, 720, 60, 5.0};

    // 1080p @ 24fps for cinematic look
    CMRenderConfig cfg_cinema = {1920, 1080, 24, 5.0};

    // Use whichever config you want
    CMScene *scene = cm_scene_create(&cfg_720p);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 1.5f});

    cm_scene_add_function(scene, func, NULL, -6, 6, 2048,
                          cm_rgba(255, 200, 100, 255), 1.0f);

    cm_scene_render_to_mp4(scene, "custom_resolution.mp4");
    cm_scene_destroy(scene);
    return 0;
}
```

---

## Use Case 9: Exploring Different Camera Zooms

```c
#include "include/cmanim.h"
#include <math.h>

float func(float x, void *ud) { return sinf(x); }

int main(void) {
    CMRenderConfig cfg = {1920, 1080, 60, 3.0};
    CMScene *scene = cm_scene_create(&cfg);

    // Zoom values:
    // 0.5 = zoomed out (see more)
    // 1.0 = normal
    // 2.0 = zoomed in (see less)
    cm_scene_set_camera(scene, (CMCamera){
        .x = 0.0f,
        .y = 0.0f,
        .zoom = 0.5f  // Try changing this!
    });

    cm_scene_add_function(scene, func, NULL, -10, 10, 4096,
                          cm_rgba(255, 150, 50, 255), 1.0f);

    cm_scene_render_to_mp4(scene, "zoom_test.mp4");
    cm_scene_destroy(scene);
    return 0;
}
```

---

## Use Case 10: Mathematical Concepts Visualization

### Fourier Series Approximation
```c
#include "include/cmanim.h"
#include <math.h>

// Square wave approximation using Fourier series
float fourier_square_wave(float x, void *userdata) {
    int *n_terms = (int*)userdata;
    float sum = 0.0f;
    for (int n = 1; n <= *n_terms; n += 2) {
        sum += (1.0f / n) * sinf(n * x);
    }
    return (4.0f / M_PI) * sum;
}

int main(void) {
    CMRenderConfig cfg = {1920, 1080, 60, 5.0};
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 0.8f});

    int n1 = 1, n3 = 3, n5 = 5, n15 = 15;

    cm_scene_add_function(scene, fourier_square_wave, &n1, -6, 6, 4096,
                          cm_rgba(255, 100, 100, 100), 1.0f);
    cm_scene_add_function(scene, fourier_square_wave, &n3, -6, 6, 4096,
                          cm_rgba(255, 150, 50, 150), 1.0f);
    cm_scene_add_function(scene, fourier_square_wave, &n5, -6, 6, 4096,
                          cm_rgba(200, 200, 50, 200), 1.0f);
    cm_scene_add_function(scene, fourier_square_wave, &n15, -6, 6, 4096,
                          cm_rgba(100, 255, 100, 255), 1.0f);

    cm_scene_render_to_mp4(scene, "fourier_series.mp4");
    cm_scene_destroy(scene);
    return 0;
}
```

---

## Building Custom Programs

### Method 1: Using the Makefile Template

Add your program to the Makefile:
```make
MY_PROGRAM := $(BIN_DIR)/my_program

$(MY_PROGRAM): my_program.c $(LIB_STATIC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LIB_STATIC) $(LDFLAGS) $(GLFW_LIBS) $(LIBS) -o $@
```

Then run:
```bash
make bin/my_program
./bin/my_program
```

### Method 2: Manual Compilation

```bash
gcc -std=c23 -O3 -Iinclude -Isrc \
    my_program.c \
    build/libcmanim.a \
    -L/opt/homebrew/lib -lglfw \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -lm \
    -o my_program

./my_program
```

---

## Color Reference

Colors are specified as RGBA with values 0-255:

```c
// Using cm_rgba()
cm_rgba(255, 0, 0, 255)      // Red
cm_rgba(0, 255, 0, 255)      // Green
cm_rgba(0, 0, 255, 255)      // Blue
cm_rgba(255, 255, 255, 255)  // White
cm_rgba(0, 0, 0, 255)        // Black
cm_rgba(255, 128, 0, 255)    // Orange
cm_rgba(128, 0, 128, 255)    // Purple
cm_rgba(0, 255, 255, 255)    // Cyan

// Using cm_rgba_f() with floats (0.0-1.0)
cm_rgba_f(1.0f, 0.0f, 0.0f, 1.0f)  // Red

// Semi-transparent (alpha < 255)
cm_rgba(255, 0, 0, 128)      // Half-transparent red
```

---

## Easing Functions

Available easing functions for smooth animations:

```c
float cm_ease_linear(float t);         // No easing
float cm_ease_in_quad(float t);        // Slow start
float cm_ease_out_quad(float t);       // Slow end
float cm_ease_in_out_quad(float t);    // Slow start and end
```

Example usage:
```c
void animate(CMScene *s, double time, void *userdata) {
    float t = time / 5.0;  // Normalize to 0-1 over 5 seconds
    float eased = cm_ease_in_out_quad(t);
    cm_curve_set_drawn_fraction(curve, eased);
}
```

---

## Troubleshooting

### "GLFW not found"
```bash
brew install glfw
```

### "ffmpeg not found"
```bash
brew install ffmpeg
```

### OpenGL deprecation warnings
Add to your compile flags:
```bash
gcc -DGL_SILENCE_DEPRECATION ...
```

### Video won't play
The MP4 should play in:
- QuickTime Player (macOS)
- VLC
- Any modern browser
- mpv, ffplay, etc.

If it doesn't play:
```bash
ffprobe graph_demo.mp4  # Check video info
```

### Render is slow
- Reduce FPS: `cfg.fps = 30` instead of 60
- Reduce resolution: `{1280, 720, 30, 5.0}`
- Reduce samples: use 1024 instead of 4096

### Function looks jagged
- Increase samples: use 4096 or 8192
- Check your function doesn't have NaN/Inf values
- Verify the x/t range covers your function properly

---

## Complete Working Directory

Your directory should look like:
```
/Users/nyuad/Desktop/canim/
├── build/libcmanim.a        ← Library
├── bin/test_render          ← Test program
├── bin/graph_demo           ← Demo program
├── test_output.png          ← Test output
├── graph_demo.mp4           ← Demo output
└── my_custom_program.c      ← Your programs here
```

---

## Next Steps

1. **Experiment**: Modify the examples above
2. **Create**: Write your own mathematical visualizations
3. **Learn**: Try different functions, parametric curves, and animations
4. **Share**: Render beautiful math animations!

For more help, see:
- `README.md` - API documentation
- `STATUS.md` - What's implemented
- `examples/graph_demo.c` - Full example
- `tests/test_render.c` - Simple example
