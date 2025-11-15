#ifndef CMANIM_FFMPEG_H
#define CMANIM_FFMPEG_H

#include <stdio.h>
#include <stdbool.h>

/* Opaque ffmpeg pipe handle */
typedef struct CMFFmpeg CMFFmpeg;

/**
 * Start ffmpeg subprocess for MP4 encoding
 *
 * @param output_filename Output MP4 file path
 * @param width Frame width
 * @param height Frame height
 * @param fps Frames per second
 * @return FFmpeg handle or NULL on failure
 */
CMFFmpeg *cm_ffmpeg_start(const char *output_filename, int width, int height, int fps);

/**
 * Write a frame to ffmpeg (RGB24 format)
 *
 * @param ff FFmpeg handle
 * @param rgb_data RGB pixel data (width * height * 3 bytes)
 * @return true on success, false on error
 */
bool cm_ffmpeg_write_frame(CMFFmpeg *ff, const unsigned char *rgb_data);

/**
 * Close ffmpeg subprocess and finalize video
 *
 * @param ff FFmpeg handle
 * @return Exit code from ffmpeg (0 on success)
 */
int cm_ffmpeg_close(CMFFmpeg *ff);

#endif /* CMANIM_FFMPEG_H */
