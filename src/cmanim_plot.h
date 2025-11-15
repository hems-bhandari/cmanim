#ifndef CMANIM_PLOT_H
#define CMANIM_PLOT_H

#include "../include/cmanim.h"
#include "cmanim_core.h"

/**
 * Sample a function y = f(x) and add vertices to a curve
 */
void cm_plot_sample_function(CMCurve *curve, CMFuncF f, void *userdata,
                             float x0, float x1, int max_samples);

/**
 * Sample a parametric curve and add vertices to a curve
 */
void cm_plot_sample_parametric(CMCurve *curve,
                               CMParamF fx, CMParamF fy, void *userdata,
                               float t0, float t1, int max_samples);

#endif /* CMANIM_PLOT_H */
