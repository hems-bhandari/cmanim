/*
 * Parametric heart shape
 * Creates: heart.mp4
 */

#include "../include/cmanim.h"
#include <math.h>
#include <stdio.h>

float heart_x(float t, void *ud) {
    return 16 * powf(sinf(t), 3) * 0.2f;
}

float heart_y(float t, void *ud) {
    return (13*cosf(t) - 5*cosf(2*t) - 2*cosf(3*t) - cosf(4*t)) * 0.2f;
}

typedef struct { CMCurve *curve; } State;

void animate(CMScene *s, double time, void *ud) {
    State *state = (State*)ud;
    float t = time / 3.0;
    cm_curve_set_drawn_fraction(state->curve, cm_ease_in_out_quad(t));
}

int main(void) {
    printf("Rendering animated heart...\n");

    CMRenderConfig cfg = {1920, 1080, 60, 3.0};
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 0.8f});

    State state;
    state.curve = cm_scene_add_parametric(scene, heart_x, heart_y, NULL,
                                          0, 2*M_PI, 4096,
                                          cm_rgba(255, 50, 150, 255), 1.0f);

    cm_scene_set_animate_callback(scene, animate, &state);
    cm_scene_render_to_mp4(scene, "heart.mp4");
    cm_scene_destroy(scene);

    printf("Done! Play with: open heart.mp4\n");
    return 0;
}
