/*
 * Basic rendering test - validates GL pipeline
 * Creates a simple scene with sin(x) and renders one frame to PNG
 */

#include "../include/cmanim.h"
#include <math.h>
#include <stdio.h>

float sin_func(float x, void *userdata) {
    (void)userdata;
    return sinf(x);
}

int main(void) {
    printf("=== cmanim basic rendering test ===\n");

    /* Create a minimal scene */
    CMRenderConfig cfg = {
        .width = 1920,
        .height = 1080,
        .fps = 60,
        .duration = 1.0
    };

    CMScene *scene = cm_scene_create(&cfg);
    if (!scene) {
        fprintf(stderr, "Failed to create scene\n");
        return 1;
    }

    /* Set camera */
    cm_scene_set_camera(scene, (CMCamera){0.0f, 0.0f, 1.5f});

    /* Add a simple sin(x) curve */
    CMCurve *curve = cm_scene_add_function(scene, sin_func, NULL,
                                           -6.0f, 6.0f, 1000,
                                           cm_rgba(255, 50, 100, 255),
                                           2.0f);

    if (!curve) {
        fprintf(stderr, "Failed to create curve\n");
        cm_scene_destroy(scene);
        return 1;
    }

    printf("Created sin(x) curve\n");

    /* Render single frame to PNG */
    printf("Rendering test frame...\n");
    int result = cm_scene_render_frame_to_png(scene, 0, "test_output.png");

    cm_scene_destroy(scene);

    if (result == 0) {
        printf("SUCCESS: Test frame written to test_output.png\n");
        return 0;
    } else {
        fprintf(stderr, "FAILED: Could not render frame\n");
        return 1;
    }
}
