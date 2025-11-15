#ifndef CMANIM_RENDERER_H
#define CMANIM_RENDERER_H

#include "cmanim_core.h"
#include <stdbool.h>

/* Opaque renderer type */
typedef struct CMRenderer CMRenderer;

/**
 * Create and initialize the renderer
 * - Creates GLFW window and OpenGL context
 * - Sets up offscreen FBO for rendering
 * - Compiles shaders
 *
 * @param width Frame width
 * @param height Frame height
 * @return Renderer instance or NULL on failure
 */
CMRenderer *cm_renderer_create(int width, int height);

/**
 * Destroy renderer and free all GPU resources
 */
void cm_renderer_destroy(CMRenderer *r);

/**
 * Begin rendering a frame
 * @param r Renderer
 * @param camera Camera parameters for view transform
 */
void cm_renderer_begin_frame(CMRenderer *r, const CMCamera *camera);

/**
 * End frame rendering
 */
void cm_renderer_end_frame(CMRenderer *r);

/**
 * Render a curve
 */
void cm_renderer_draw_curve(CMRenderer *r, CMCurve *curve);

/**
 * Render axes and ticks
 */
void cm_renderer_draw_axes(CMRenderer *r, const CMAxes *axes);

/**
 * Read the current framebuffer into RGB buffer
 * @param r Renderer
 * @param buffer Output buffer (must be width*height*3 bytes)
 */
void cm_renderer_read_pixels(CMRenderer *r, unsigned char *buffer);

/**
 * Get renderer framebuffer width
 */
int cm_renderer_get_width(const CMRenderer *r);

/**
 * Get renderer framebuffer height
 */
int cm_renderer_get_height(const CMRenderer *r);

#endif /* CMANIM_RENDERER_H */
