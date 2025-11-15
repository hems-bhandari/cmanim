# cmanim - C Math Animation Library

A lightweight C23 library for rendering 2D mathematical animations using OpenGL, with output to high-quality MP4 video files via FFmpeg.

## Features

- **2D Function Plotting**: Plot functions y = f(x) with automatic sampling
- **Parametric Curves**: Render parametric curves (x(t), y(t))
- **Animation**: Time-based animation with easing functions
- **High Quality Output**: Renders at 1080p (1920×1080) by default, 60 FPS
- **MP4 Export**: Direct encoding to MP4 via FFmpeg
- **Simple API**: Clean C23 API with minimal dependencies

## Requirements

### System Dependencies

**macOS:**
```bash
brew install glfw ffmpeg
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install libglfw3-dev libfreetype6-dev ffmpeg
```

**Linux (Fedora):**
```bash
sudo dnf install glfw-devel freetype-devel ffmpeg
```

### Build Tools

- GCC or Clang with C23 support (`-std=c23`)
- Make

## Building

```bash
# Clone or navigate to the repository
cd canim

# Build the library and test
make

# Run the basic rendering test
make test

# Build and run the demo (creates graph_demo.mp4)
make demo
make run-demo
```

## Quick Start

```c
#include <cmanim.h>
#include <math.h>

float my_function(float x, void *userdata) {
    return sinf(x);
}

int main(void) {
    // Configure rendering
    CMRenderConfig cfg = {
        .width = 1920,
        .height = 1080,
        .fps = 60,
        .duration = 10.0  // 10 seconds
    };

    // Create scene
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0.0f, 0.0f, 1.5f});

    // Add a function plot
    cm_scene_add_function(scene, my_function, NULL,
                          -6.0f, 6.0f,           // x range
                          4096,                   // sample points
                          cm_rgba(255, 50, 100, 255),  // color
                          2.0f);                  // line width

    // Render to MP4
    cm_scene_render_to_mp4(scene, "output.mp4");

    cm_scene_destroy(scene);
    return 0;
}
```

## API Overview

### Scene Management

- `CMScene *cm_scene_create(const CMRenderConfig *cfg)` - Create a new scene
- `void cm_scene_destroy(CMScene *s)` - Destroy scene and free resources
- `void cm_scene_set_camera(CMScene *s, CMCamera cam)` - Set camera position/zoom

### Plotting Functions

- `cm_scene_add_function()` - Add y = f(x) plot
- `cm_scene_add_parametric()` - Add parametric curve (x(t), y(t))
- `cm_curve_set_drawn_fraction()` - Animate curve drawing (0.0 to 1.0)

### Rendering

- `cm_scene_render_to_mp4()` - Render full animation to MP4
- `cm_scene_render_frame_to_png()` - Render single frame (for debugging)

### Animation

- `cm_scene_set_animate_callback()` - Set per-frame animation callback
- Easing functions: `cm_ease_linear()`, `cm_ease_in_quad()`, `cm_ease_out_quad()`, `cm_ease_in_out_quad()`

## Project Structure

```
canim/
├── include/
│   ├── cmanim.h              # Public API header
│   ├── stb_image_write.h     # PNG output (single-header library)
│   └── KHR/khrplatform.h     # OpenGL platform types
├── src/
│   ├── cmanim_core.c/.h      # Scene management, data structures
│   ├── cmanim_renderer.c/.h # OpenGL rendering
│   ├── cmanim_plot.c/h       # Function sampling and plotting
│   ├── cmanim_render.c       # High-level rendering loop
│   ├── cmanim_ffmpeg.c/h     # FFmpeg integration
│   └── gl_loader.c/h         # Minimal OpenGL function loader
├── examples/
│   └── graph_demo.c          # Example animation
├── tests/
│   └── test_render.c         # Basic rendering test
├── Makefile
└── README.md
```

## Examples

### Animated Function Drawing

```c
void animate_callback(CMScene *s, double time, void *userdata) {
    CMCurve *curve = (CMCurve*)userdata;
    float progress = time / 5.0;  // 5 second animation
    cm_curve_set_drawn_fraction(curve, cm_ease_in_out_quad(progress));
}

// In main:
CMCurve *curve = cm_scene_add_function(scene, sin_func, NULL, -6, 6, 4096, color, 2.0f);
cm_scene_set_animate_callback(scene, animate_callback, curve);
```

### Parametric Lissajous Curve

```c
float lissajous_x(float t, void *ud) { return 2.0f * sinf(3.0f * t); }
float lissajous_y(float t, void *ud) { return 1.0f * sinf(4.0f * t + 0.5f); }

cm_scene_add_parametric(scene, lissajous_x, lissajous_y, NULL,
                        0.0f, 2*M_PI, 8192,
                        cm_rgba(51, 204, 136, 255), 2.0f);
```

## Technical Details

- **Rendering Backend**: OpenGL 3.3 Core Profile
- **Window Management**: GLFW 3.x
- **Resolution**: 1920×1080 (configurable)
- **Frame Rate**: 60 FPS (configurable)
- **Video Encoding**: H.264 via FFmpeg (CRF 18, medium preset)
- **Coordinate System**: Orthographic projection, world coordinates

## Troubleshooting

### "GLFW not found"
Install GLFW: `brew install glfw` (macOS) or `apt-get install libglfw3-dev` (Linux)

### "ffmpeg not found"
Install FFmpeg: `brew install ffmpeg` (macOS) or `apt-get install ffmpeg` (Linux)

### Rendering appears black/empty
- Check that function samples are finite (no NaN/inf values)
- Verify camera zoom and position cover the function range
- Try rendering a test PNG first: `cm_scene_render_frame_to_png(scene, 0, "debug.png")`

## Performance

- Typical performance: 60+ FPS @ 1080p on Apple M1
- Function sampling is CPU-bound; use appropriate `max_samples` (1000-8192)
- FFmpeg encoding runs in real-time or faster with hardware acceleration

## License

This is a reference implementation created for educational purposes.

## Roadmap / Future Enhancements

- [ ] Text rendering with FreeType (tick labels, annotations)
- [ ] Axes with automatic tick placement
- [ ] Thick line rendering (stroke triangulation)
- [ ] Anti-aliasing (MSAA/FXAA)
- [ ] Adaptive curve sampling (curvature-based)
- [ ] Python bindings
- [ ] More easing functions
- [ ] Color gradients
- [ ] Multiple scenes/transitions

## Contributing

This is an MVP implementation. Contributions welcome for:
- Cross-platform testing (Linux, Windows)
- Additional animation primitives
- Performance optimizations
- Example scenes
