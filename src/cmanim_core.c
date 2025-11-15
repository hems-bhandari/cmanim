#include "cmanim_core.h"
#include "gl_loader.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/* ============================================================================
 * Curve Management
 * ========================================================================= */

CMCurve *cm_curve_create(uint32_t color, float line_width) {
    CMCurve *c = (CMCurve*)calloc(1, sizeof(CMCurve));
    if (!c) return NULL;

    c->color = color;
    c->line_width = line_width;
    c->drawn_fraction = 1.0f;
    c->vertices_allocated = 64;
    c->vertices = (CMVertex*)malloc(c->vertices_allocated * sizeof(CMVertex));
    c->gpu_dirty = true;
    c->vao = 0;
    c->vbo = 0;
    c->next = NULL;

    if (!c->vertices) {
        free(c);
        return NULL;
    }

    return c;
}

void cm_curve_destroy(CMCurve *c) {
    if (!c) return;

    if (c->vbo) glDeleteBuffers(1, &c->vbo);
    if (c->vao) glDeleteVertexArrays(1, &c->vao);

    free(c->vertices);
    free(c);
}

void cm_curve_add_vertex(CMCurve *c, float x, float y) {
    if (!c) return;

    /* Resize if needed */
    if (c->vertex_count >= c->vertices_allocated) {
        int new_size = c->vertices_allocated * 2;
        CMVertex *new_verts = (CMVertex*)realloc(c->vertices, new_size * sizeof(CMVertex));
        if (!new_verts) {
            fprintf(stderr, "Failed to allocate vertex memory\n");
            return;
        }
        c->vertices = new_verts;
        c->vertices_allocated = new_size;
    }

    c->vertices[c->vertex_count].x = x;
    c->vertices[c->vertex_count].y = y;
    c->vertex_count++;
    c->gpu_dirty = true;
}

void cm_curve_reserve(CMCurve *c, int count) {
    if (!c || count <= c->vertices_allocated) return;

    CMVertex *new_verts = (CMVertex*)realloc(c->vertices, count * sizeof(CMVertex));
    if (!new_verts) {
        fprintf(stderr, "Failed to reserve vertex memory\n");
        return;
    }
    c->vertices = new_verts;
    c->vertices_allocated = count;
}

void cm_curve_upload_to_gpu(CMCurve *c) {
    if (!c || !c->gpu_dirty) return;

    if (!c->vao) {
        glGenVertexArrays(1, &c->vao);
        glGenBuffers(1, &c->vbo);
    }

    glBindVertexArray(c->vao);
    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    glBufferData(GL_ARRAY_BUFFER, c->vertex_count * sizeof(CMVertex),
                 c->vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CMVertex), (void*)0);

    glBindVertexArray(0);
    c->gpu_dirty = false;
}

/* ============================================================================
 * Scene Management
 * ========================================================================= */

CMScene *cm_scene_create(const CMRenderConfig *cfg) {
    if (!cfg) return NULL;

    CMScene *s = (CMScene*)calloc(1, sizeof(CMScene));
    if (!s) return NULL;

    s->config = *cfg;
    s->camera = (CMCamera){0.0f, 0.0f, 1.0f};
    s->curves = NULL;
    s->current_time = 0.0;
    s->animate_callback = NULL;
    s->animate_userdata = NULL;
    s->axes.enabled = false;

    return s;
}

void cm_scene_destroy(CMScene *s) {
    if (!s) return;

    /* Free all curves */
    CMCurve *cur = s->curves;
    while (cur) {
        CMCurve *next = cur->next;
        cm_curve_destroy(cur);
        cur = next;
    }

    free(s);
}

void cm_scene_set_camera(CMScene *s, CMCamera cam) {
    if (s) {
        s->camera = cam;
    }
}

double cm_scene_get_time(const CMScene *s) {
    return s ? s->current_time : 0.0;
}

void cm_scene_set_animate_callback(CMScene *s, CMAnimateFunc func, void *userdata) {
    if (s) {
        s->animate_callback = func;
        s->animate_userdata = userdata;
    }
}

/* ============================================================================
 * Curve Modification API
 * ========================================================================= */

void cm_curve_set_drawn_fraction(CMCurve *c, float fraction) {
    if (c) {
        c->drawn_fraction = fmaxf(0.0f, fminf(1.0f, fraction));
    }
}

void cm_curve_set_style(CMCurve *c, uint32_t color_rgba, float width) {
    if (c) {
        c->color = color_rgba;
        c->line_width = width;
    }
}

/* ============================================================================
 * Axes Management
 * ========================================================================= */

void cm_scene_add_axes(CMScene *s, float x_min, float x_max,
                       float y_min, float y_max,
                       int x_ticks, int y_ticks,
                       const char *font_name, int font_size) {
    if (!s) return;

    s->axes.x_min = x_min;
    s->axes.x_max = x_max;
    s->axes.y_min = y_min;
    s->axes.y_max = y_max;
    s->axes.x_ticks = x_ticks;
    s->axes.y_ticks = y_ticks;
    s->axes.font_size = font_size;
    s->axes.enabled = true;

    if (font_name) {
        strncpy(s->axes.font_name, font_name, sizeof(s->axes.font_name) - 1);
        s->axes.font_name[sizeof(s->axes.font_name) - 1] = '\0';
    } else {
        strcpy(s->axes.font_name, "default");
    }
}

/* ============================================================================
 * Easing Functions
 * ========================================================================= */

float cm_ease_linear(float t) {
    return t;
}

float cm_ease_in_quad(float t) {
    return t * t;
}

float cm_ease_out_quad(float t) {
    return t * (2.0f - t);
}

float cm_ease_in_out_quad(float t) {
    if (t < 0.5f) {
        return 2.0f * t * t;
    } else {
        return -1.0f + (4.0f - 2.0f * t) * t;
    }
}
