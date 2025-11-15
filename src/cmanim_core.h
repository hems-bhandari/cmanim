#ifndef CMANIM_CORE_H
#define CMANIM_CORE_H

#include "../include/cmanim.h"
#include <stdlib.h>

/* ============================================================================
 * Internal Data Structures
 * ========================================================================= */

/**
 * Vertex data for curves
 */
typedef struct {
    float x, y;
} CMVertex;

/**
 * Curve data structure
 */
struct CMCurve {
    CMVertex *vertices;      /* Array of vertices */
    int vertex_count;        /* Total vertices */
    int vertices_allocated;  /* Allocated capacity */

    uint32_t color;          /* RGBA color */
    float line_width;        /* Line width */
    float drawn_fraction;    /* How much of curve to draw (0-1) */

    /* GPU resources */
    unsigned int vao;
    unsigned int vbo;
    bool gpu_dirty;          /* True if VBO needs update */

    struct CMCurve *next;    /* Linked list */
};

/**
 * Axes data
 */
typedef struct {
    float x_min, x_max;
    float y_min, y_max;
    int x_ticks, y_ticks;
    char font_name[64];
    int font_size;
    bool enabled;
} CMAxes;

/**
 * Scene structure
 */
struct CMScene {
    CMRenderConfig config;
    CMCamera camera;
    CMAxes axes;

    /* Curves and objects */
    CMCurve *curves;         /* Linked list of curves */

    /* Animation */
    double current_time;
    CMAnimateFunc animate_callback;
    void *animate_userdata;

    /* Rendering state */
    void *renderer;          /* Opaque renderer pointer */
};

/* ============================================================================
 * Core Functions
 * ========================================================================= */

/**
 * Create a new curve
 */
CMCurve *cm_curve_create(uint32_t color, float line_width);

/**
 * Destroy a curve and free resources
 */
void cm_curve_destroy(CMCurve *c);

/**
 * Add a vertex to a curve
 */
void cm_curve_add_vertex(CMCurve *c, float x, float y);

/**
 * Reserve space for vertices
 */
void cm_curve_reserve(CMCurve *c, int count);

/**
 * Upload curve data to GPU
 */
void cm_curve_upload_to_gpu(CMCurve *c);

/**
 * Easing function implementations
 */
float cm_ease_linear_impl(float t);
float cm_ease_in_quad_impl(float t);
float cm_ease_out_quad_impl(float t);
float cm_ease_in_out_quad_impl(float t);

#endif /* CMANIM_CORE_H */
