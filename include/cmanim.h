#ifndef CMANIM_H
#define CMANIM_H

/**
 * cmanim - C library for 2D math animations
 *
 * A minimal library for rendering 2D function plots and parametric curves
 * with animation support, outputting to MP4 via OpenGL and ffmpeg.
 */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Core Types
 * ========================================================================= */

typedef float cfloat;

/**
 * Render configuration
 */
typedef struct {
    int width;      /* Frame width in pixels (default: 1920) */
    int height;     /* Frame height in pixels (default: 1080) */
    int fps;        /* Frames per second (default: 60) */
    double duration; /* Animation duration in seconds */
} CMRenderConfig;

/**
 * Camera (orthographic projection)
 */
typedef struct {
    float x, y;     /* Center position in world coordinates */
    float zoom;     /* Zoom level (scale factor) */
} CMCamera;

/* Opaque types */
typedef struct CMScene CMScene;
typedef struct CMCurve CMCurve;

/* ============================================================================
 * Scene Management
 * ========================================================================= */

/**
 * Create a new scene with the given render configuration
 */
CMScene *cm_scene_create(const CMRenderConfig *cfg);

/**
 * Destroy a scene and free all resources
 */
void cm_scene_destroy(CMScene *s);

/**
 * Set camera parameters for the scene
 */
void cm_scene_set_camera(CMScene *s, CMCamera cam);

/**
 * Get current animation time (updated during rendering)
 */
double cm_scene_get_time(const CMScene *s);

/* ============================================================================
 * Function & Curve Plotting
 * ========================================================================= */

/**
 * Function callback for y = f(x) plots
 * @param x The input value
 * @param userdata User-provided data pointer
 * @return The function value at x
 */
typedef float (*CMFuncF)(float x, void *userdata);

/**
 * Parametric function callback for x(t) or y(t)
 * @param t Parameter value
 * @param userdata User-provided data pointer
 * @return The coordinate value at parameter t
 */
typedef float (*CMParamF)(float t, void *userdata);

/**
 * Add a 2D function plot y = f(x) to the scene
 *
 * @param s Scene
 * @param f Function to evaluate
 * @param userdata User data passed to function
 * @param x0 Start of x range
 * @param x1 End of x range
 * @param max_samples Maximum number of sample points
 * @param color_rgba Color in RGBA format (0xRRGGBBAA)
 * @param line_width Line width in pixels
 * @return Curve handle for further modification
 */
CMCurve *cm_scene_add_function(CMScene *s, CMFuncF f, void *userdata,
                               float x0, float x1, int max_samples,
                               uint32_t color_rgba, float line_width);

/**
 * Add a parametric curve (x(t), y(t)) to the scene
 *
 * @param s Scene
 * @param fx Function for x coordinate
 * @param fy Function for y coordinate
 * @param userdata User data passed to functions
 * @param t0 Start of parameter range
 * @param t1 End of parameter range
 * @param max_samples Maximum number of sample points
 * @param color_rgba Color in RGBA format (0xRRGGBBAA)
 * @param line_width Line width in pixels
 * @return Curve handle for further modification
 */
CMCurve *cm_scene_add_parametric(CMScene *s,
                                 CMParamF fx, CMParamF fy, void *userdata,
                                 float t0, float t1, int max_samples,
                                 uint32_t color_rgba, float line_width);

/**
 * Set how much of the curve is drawn (for animation)
 * @param c Curve handle
 * @param fraction Fraction of curve to draw (0.0 to 1.0)
 */
void cm_curve_set_drawn_fraction(CMCurve *c, float fraction);

/**
 * Update curve visual style
 * @param c Curve handle
 * @param color_rgba New color
 * @param width New line width
 */
void cm_curve_set_style(CMCurve *c, uint32_t color_rgba, float width);

/* ============================================================================
 * Axes & Annotations
 * ========================================================================= */

/**
 * Add coordinate axes with ticks and labels to the scene
 *
 * @param s Scene
 * @param x_min Minimum x value for x-axis
 * @param x_max Maximum x value for x-axis
 * @param y_min Minimum y value for y-axis
 * @param y_max Maximum y value for y-axis
 * @param x_ticks Number of tick marks on x-axis
 * @param y_ticks Number of tick marks on y-axis
 * @param font_name Font name (or "default")
 * @param font_size Font size in points
 */
void cm_scene_add_axes(CMScene *s, float x_min, float x_max,
                       float y_min, float y_max,
                       int x_ticks, int y_ticks,
                       const char *font_name, int font_size);

/* ============================================================================
 * Animation & Rendering
 * ========================================================================= */

/**
 * Animation update callback - called before rendering each frame
 * @param s Scene
 * @param time Current animation time in seconds
 * @param userdata User data
 */
typedef void (*CMAnimateFunc)(CMScene *s, double time, void *userdata);

/**
 * Set animation callback
 * @param s Scene
 * @param func Callback function (can be NULL)
 * @param userdata User data to pass to callback
 */
void cm_scene_set_animate_callback(CMScene *s, CMAnimateFunc func, void *userdata);

/**
 * Render the scene to MP4 file
 *
 * Renders all frames according to the render config and pipes them
 * to ffmpeg to create an MP4 file.
 *
 * @param s Scene
 * @param output_filename Output MP4 filename
 * @return 0 on success, negative on error
 */
int cm_scene_render_to_mp4(CMScene *s, const char *output_filename);

/**
 * Render a single frame to PNG (for debugging)
 * @param s Scene
 * @param frame_index Frame number
 * @param output_filename Output PNG filename
 * @return 0 on success, negative on error
 */
int cm_scene_render_frame_to_png(CMScene *s, int frame_index, const char *output_filename);

/* ============================================================================
 * Utility Functions
 * ========================================================================= */

/**
 * Create RGBA color from float components (0.0-1.0)
 */
static inline uint32_t cm_rgba_f(float r, float g, float b, float a) {
    uint32_t ri = (uint32_t)(r * 255.0f) & 0xFF;
    uint32_t gi = (uint32_t)(g * 255.0f) & 0xFF;
    uint32_t bi = (uint32_t)(b * 255.0f) & 0xFF;
    uint32_t ai = (uint32_t)(a * 255.0f) & 0xFF;
    return (ri << 24) | (gi << 16) | (bi << 8) | ai;
}

/**
 * Create RGBA color from byte components (0-255)
 */
static inline uint32_t cm_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | (uint32_t)a;
}

/**
 * Easing functions for smooth animation
 */
float cm_ease_linear(float t);
float cm_ease_in_quad(float t);
float cm_ease_out_quad(float t);
float cm_ease_in_out_quad(float t);

#ifdef __cplusplus
}
#endif

#endif /* CMANIM_H */
