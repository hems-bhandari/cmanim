/*
 * Spiral drawing animation
 * Creates: spiral.mp4
 */

#include "../include/cmanim.h"
#include <math.h>
#include <stdio.h>

float spiral_x(float t, void *ud) { return t * cosf(t) * 0.3f; }
float spiral_y(float t, void *ud) { return t * sinf(t) * 0.3f; }

typedef struct { CMCurve *curve; } State;

void animate(CMScene *s, double time, void *ud) {
    State *state = (State*)ud;
    float t = time / 5.0;
    cm_curve_set_drawn_fraction(state->curve, cm_ease_out_quad(t));
}

int main(void) {
    printf("Rendering spiral...\n");

    CMRenderConfig cfg = {1920, 1080, 60, 5.0};
    CMScene *scene = cm_scene_create(&cfg);
    cm_scene_set_camera(scene, (CMCamera){0, 0, 0.6f});

    State state;
    state.curve = cm_scene_add_parametric(scene, spiral_x, spiral_y, NULL,
                                          0, 6*M_PI, 8192,
                                          cm_rgba(100, 200, 255, 255), 1.0f);

    cm_scene_set_animate_callback(scene, animate, &state);
    cm_scene_render_to_mp4(scene, "spiral.mp4");
    cm_scene_destroy(scene);

    printf("Done! Play with: open spiral.mp4\n");
    return 0;
}
