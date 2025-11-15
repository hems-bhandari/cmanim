/*
 * Multiple sine waves with different frequencies
 * Creates: waves.mp4
 */

#include "../include/cmanim.h"
#include <math.h>
#include <stdio.h>

typedef struct {
    float amplitude;
    float frequency;
} WaveParams;

float wave(float x, void *ud) {
    WaveParams *p = (WaveParams*)ud;
    return p->amplitude * sinf(p->frequency * x);
}

int main(void) {
    printf("Rendering multiple waves...\n");

    CMRenderConfig cfg = {1920, 1080, 60, 5.0};
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 1.5f});

    WaveParams w1 = {1.0f, 1.0f};
    WaveParams w2 = {0.7f, 2.0f};
    WaveParams w3 = {0.5f, 3.0f};

    cm_scene_add_function(scene, wave, &w1, -6, 6, 2048,
                          cm_rgba(255, 50, 50, 255), 1.0f);
    cm_scene_add_function(scene, wave, &w2, -6, 6, 2048,
                          cm_rgba(50, 255, 50, 255), 1.0f);
    cm_scene_add_function(scene, wave, &w3, -6, 6, 2048,
                          cm_rgba(50, 50, 255, 255), 1.0f);

    cm_scene_render_to_mp4(scene, "waves.mp4");
    cm_scene_destroy(scene);

    printf("Done! Play with: open waves.mp4\n");
    return 0;
}
