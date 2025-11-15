/*
 * Simple sine wave - most basic example
 * Creates: simple_sine.mp4
 */

#include "../include/cmanim.h"
#include <math.h>
#include <stdio.h>

float sine(float x, void *ud) {
    return sinf(x);
}

int main(void) {
    printf("Rendering simple sine wave...\n");

    CMRenderConfig cfg = {1920, 1080, 60, 3.0};
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 1.5f});

    cm_scene_add_function(scene, sine, NULL, -6, 6, 2048,
                          cm_rgba(255, 50, 100, 255), 1.0f);

    cm_scene_render_to_mp4(scene, "simple_sine.mp4");
    cm_scene_destroy(scene);

    printf("Done! Play with: open simple_sine.mp4\n");
    return 0;
}
