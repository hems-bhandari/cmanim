/*
 * graph_demo.c - Example animation demonstrating cmanim
 *
 * Creates a 10-second animation featuring:
 * - Animated drawing of sin(x)
 * - Parametric Lissajous curve
 * - Coordinate axes (placeholder for now)
 *
 * Output: graph_demo.mp4 (1920x1080, 60fps)
 */

#include "../include/cmanim.h"
#include <math.h>
#include <stdio.h>

/* Function definitions */
float sin_f(float x, void *ud) {
    (void)ud;
    return sinf(x);
}

float lissajous_x(float t, void *ud) {
    (void)ud;
    return 2.0f * sinf(3.0f * t);
}

float lissajous_y(float t, void *ud) {
    (void)ud;
    return 1.0f * sinf(4.0f * t + 0.5f);
}

/* Animation state */
typedef struct {
    CMCurve *sin_curve;
    CMCurve *lissajous_curve;
} AnimState;

/* Animation callback - called every frame */
void animate_frame(CMScene *s, double time, void *userdata) {
    AnimState *state = (AnimState*)userdata;

    /* Phase 1 (0-4s): Draw sin curve */
    if (time < 4.0) {
        float progress = time / 4.0;
        float fraction = cm_ease_in_out_quad(progress);
        cm_curve_set_drawn_fraction(state->sin_curve, fraction);
        cm_curve_set_drawn_fraction(state->lissajous_curve, 0.0f);
    }
    /* Phase 2 (4-10s): Keep sin, draw Lissajous */
    else {
        cm_curve_set_drawn_fraction(state->sin_curve, 1.0f);
        float progress = (time - 4.0) / 6.0;
        float fraction = cm_ease_in_out_quad(progress);
        cm_curve_set_drawn_fraction(state->lissajous_curve, fraction);
    }
}

int main(void) {
    printf("=== cmanim Graph Demo ===\n");
    printf("Creating 10-second animation at 1080p, 60fps\n\n");

    /* Configure rendering: 10 second animation at 1080p60 */
    CMRenderConfig cfg = {
        .width = 1920,
        .height = 1080,
        .fps = 60,
        .duration = 10.0
    };

    /* Create scene */
    CMScene *scene = cm_scene_create(&cfg);
    if (!scene) {
        fprintf(stderr, "Failed to create scene\n");
        return 1;
    }

    /* Set camera to view range ~[-6,6] x [-4,4] in world coords */
    cm_scene_set_camera(scene, (CMCamera){
        .x = 0.0f,
        .y = 0.0f,
        .zoom = 1.5f
    });

    /* Add axes (placeholder - not fully implemented yet) */
    cm_scene_add_axes(scene, -6.0f, 6.0f, -3.0f, 3.0f, 12, 8, "default", 18);

    /* Create animation state */
    AnimState state = {0};

    /* Add sin(x) curve on [-6, 6] */
    state.sin_curve = cm_scene_add_function(
        scene,
        sin_f,
        NULL,
        -6.0f, 6.0f,
        4096,
        cm_rgba(255, 51, 102, 255),  // Red-pink color
        1.0f
    );

    if (!state.sin_curve) {
        fprintf(stderr, "Failed to create sin curve\n");
        cm_scene_destroy(scene);
        return 1;
    }

    /* Add Lissajous parametric curve */
    state.lissajous_curve = cm_scene_add_parametric(
        scene,
        lissajous_x,
        lissajous_y,
        NULL,
        0.0f, 2.0f * M_PI,
        8192,
        cm_rgba(51, 204, 136, 255),  // Green-cyan color
        1.0f
    );

    if (!state.lissajous_curve) {
        fprintf(stderr, "Failed to create Lissajous curve\n");
        cm_scene_destroy(scene);
        return 1;
    }

    printf("Created curves:\n");
    printf("  - sin(x) on [-6, 6]\n");
    printf("  - Lissajous parametric curve\n\n");

    /* Set animation callback */
    cm_scene_set_animate_callback(scene, animate_frame, &state);

    /* Render to MP4 */
    printf("Starting render...\n");
    int result = cm_scene_render_to_mp4(scene, "graph_demo.mp4");

    /* Cleanup */
    cm_scene_destroy(scene);

    if (result == 0) {
        printf("\n=== SUCCESS ===\n");
        printf("Animation saved to: graph_demo.mp4\n");
        printf("Play with: open graph_demo.mp4\n");
        return 0;
    } else {
        fprintf(stderr, "\n=== FAILED ===\n");
        fprintf(stderr, "Rendering failed with code %d\n", result);
        return 1;
    }
}
