#include "../include/cmanim.h"
#include "cmanim_core.h"
#include "cmanim_renderer.h"
#include "cmanim_ffmpeg.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>

/* ============================================================================
 * Single Frame Rendering (PNG)
 * ========================================================================= */

int cm_scene_render_frame_to_png(CMScene *s, int frame_index, const char *output_filename) {
    if (!s || !output_filename) return -1;

    /* Create renderer if not exists */
    if (!s->renderer) {
        s->renderer = cm_renderer_create(s->config.width, s->config.height);
        if (!s->renderer) {
            fprintf(stderr, "Failed to create renderer\n");
            return -1;
        }
    }

    CMRenderer *r = (CMRenderer*)s->renderer;

    /* Calculate time for this frame */
    s->current_time = (double)frame_index / (double)s->config.fps;

    /* Call animation callback */
    if (s->animate_callback) {
        s->animate_callback(s, s->current_time, s->animate_userdata);
    }

    /* Render frame */
    cm_renderer_begin_frame(r, &s->camera);

    /* Draw all curves */
    for (CMCurve *c = s->curves; c != NULL; c = c->next) {
        cm_renderer_draw_curve(r, c);
    }

    /* Draw axes if enabled */
    if (s->axes.enabled) {
        cm_renderer_draw_axes(r, &s->axes);
    }

    cm_renderer_end_frame(r);

    /* Read pixels */
    size_t buffer_size = s->config.width * s->config.height * 3;
    unsigned char *pixels = (unsigned char*)malloc(buffer_size);
    if (!pixels) {
        fprintf(stderr, "Failed to allocate pixel buffer\n");
        return -1;
    }

    cm_renderer_read_pixels(r, pixels);

    /* Flip image vertically (OpenGL has origin at bottom-left) */
    int width = s->config.width;
    int height = s->config.height;
    unsigned char *flipped = (unsigned char*)malloc(buffer_size);
    for (int y = 0; y < height; y++) {
        memcpy(flipped + y * width * 3,
               pixels + (height - 1 - y) * width * 3,
               width * 3);
    }

    /* Write PNG */
    int result = stbi_write_png(output_filename, width, height, 3, flipped, width * 3);

    free(pixels);
    free(flipped);

    if (!result) {
        fprintf(stderr, "Failed to write PNG: %s\n", output_filename);
        return -1;
    }

    printf("Rendered frame %d to %s\n", frame_index, output_filename);
    return 0;
}

/* ============================================================================
 * Full Animation Rendering (MP4)
 * ========================================================================= */

int cm_scene_render_to_mp4(CMScene *s, const char *output_filename) {
    if (!s || !output_filename) return -1;

    /* Create renderer */
    if (!s->renderer) {
        s->renderer = cm_renderer_create(s->config.width, s->config.height);
        if (!s->renderer) {
            fprintf(stderr, "Failed to create renderer\n");
            return -1;
        }
    }

    CMRenderer *r = (CMRenderer*)s->renderer;

    /* Calculate total frames */
    int total_frames = (int)(s->config.duration * s->config.fps);
    printf("Rendering %d frames at %d fps (%.2fs)...\n",
           total_frames, s->config.fps, s->config.duration);

    /* Start ffmpeg */
    CMFFmpeg *ff = cm_ffmpeg_start(output_filename,
                                   s->config.width, s->config.height,
                                   s->config.fps);
    if (!ff) {
        fprintf(stderr, "Failed to start ffmpeg\n");
        return -1;
    }

    /* Allocate pixel buffer */
    size_t buffer_size = s->config.width * s->config.height * 3;
    unsigned char *pixels = (unsigned char*)malloc(buffer_size);
    unsigned char *flipped = (unsigned char*)malloc(buffer_size);

    if (!pixels || !flipped) {
        fprintf(stderr, "Failed to allocate pixel buffers\n");
        free(pixels);
        free(flipped);
        cm_ffmpeg_close(ff);
        return -1;
    }

    /* Render each frame */
    for (int frame = 0; frame < total_frames; frame++) {
        /* Update time */
        s->current_time = (double)frame / (double)s->config.fps;

        /* Call animation callback */
        if (s->animate_callback) {
            s->animate_callback(s, s->current_time, s->animate_userdata);
        }

        /* Render frame */
        cm_renderer_begin_frame(r, &s->camera);

        /* Draw all curves */
        for (CMCurve *c = s->curves; c != NULL; c = c->next) {
            cm_renderer_draw_curve(r, c);
        }

        /* Draw axes if enabled */
        if (s->axes.enabled) {
            cm_renderer_draw_axes(r, &s->axes);
        }

        cm_renderer_end_frame(r);

        /* Read pixels and flip vertically */
        cm_renderer_read_pixels(r, pixels);

        int width = s->config.width;
        int height = s->config.height;
        for (int y = 0; y < height; y++) {
            memcpy(flipped + y * width * 3,
                   pixels + (height - 1 - y) * width * 3,
                   width * 3);
        }

        /* Write to ffmpeg */
        if (!cm_ffmpeg_write_frame(ff, flipped)) {
            fprintf(stderr, "Failed to write frame %d\n", frame);
            break;
        }

        /* Progress indication */
        if ((frame + 1) % 60 == 0 || frame == total_frames - 1) {
            printf("  Progress: %d/%d frames (%.1f%%)\n",
                   frame + 1, total_frames,
                   100.0f * (frame + 1) / total_frames);
        }
    }

    free(pixels);
    free(flipped);

    /* Close ffmpeg and check result */
    int exit_code = cm_ffmpeg_close(ff);

    if (exit_code == 0) {
        printf("Successfully rendered to: %s\n", output_filename);
    } else {
        fprintf(stderr, "Rendering failed with code %d\n", exit_code);
    }

    return exit_code;
}
