#include "cmanim_plot.h"
#include <math.h>
#include <stdio.h>

/* ============================================================================
 * Function Sampling
 * ========================================================================= */

void cm_plot_sample_function(CMCurve *curve, CMFuncF f, void *userdata,
                             float x0, float x1, int max_samples) {
    if (!curve || !f || max_samples < 2) return;

    /* Reserve space */
    cm_curve_reserve(curve, max_samples);

    /* Uniform sampling */
    float dx = (x1 - x0) / (float)(max_samples - 1);

    for (int i = 0; i < max_samples; i++) {
        float x = x0 + i * dx;
        float y = f(x, userdata);

        /* Skip NaN and infinite values */
        if (!isfinite(y)) continue;

        cm_curve_add_vertex(curve, x, y);
    }
}

/* ============================================================================
 * Parametric Curve Sampling
 * ========================================================================= */

void cm_plot_sample_parametric(CMCurve *curve,
                               CMParamF fx, CMParamF fy, void *userdata,
                               float t0, float t1, int max_samples) {
    if (!curve || !fx || !fy || max_samples < 2) return;

    /* Reserve space */
    cm_curve_reserve(curve, max_samples);

    /* Uniform sampling */
    float dt = (t1 - t0) / (float)(max_samples - 1);

    for (int i = 0; i < max_samples; i++) {
        float t = t0 + i * dt;
        float x = fx(t, userdata);
        float y = fy(t, userdata);

        /* Skip NaN and infinite values */
        if (!isfinite(x) || !isfinite(y)) continue;

        cm_curve_add_vertex(curve, x, y);
    }
}

/* ============================================================================
 * Public API Implementation
 * ========================================================================= */

CMCurve *cm_scene_add_function(CMScene *s, CMFuncF f, void *userdata,
                               float x0, float x1, int max_samples,
                               uint32_t color_rgba, float line_width) {
    if (!s || !f) return NULL;

    CMCurve *curve = cm_curve_create(color_rgba, line_width);
    if (!curve) return NULL;

    /* Sample the function */
    cm_plot_sample_function(curve, f, userdata, x0, x1, max_samples);

    /* Add to scene's curve list */
    curve->next = s->curves;
    s->curves = curve;

    return curve;
}

CMCurve *cm_scene_add_parametric(CMScene *s,
                                 CMParamF fx, CMParamF fy, void *userdata,
                                 float t0, float t1, int max_samples,
                                 uint32_t color_rgba, float line_width) {
    if (!s || !fx || !fy) return NULL;

    CMCurve *curve = cm_curve_create(color_rgba, line_width);
    if (!curve) return NULL;

    /* Sample the parametric curve */
    cm_plot_sample_parametric(curve, fx, fy, userdata, t0, t1, max_samples);

    /* Add to scene's curve list */
    curve->next = s->curves;
    s->curves = curve;

    return curve;
}
