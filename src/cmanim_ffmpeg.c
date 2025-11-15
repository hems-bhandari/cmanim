#include "cmanim_ffmpeg.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

struct CMFFmpeg {
    FILE *pipe;
    pid_t pid;
    int width, height;
    size_t frame_size;
};

CMFFmpeg *cm_ffmpeg_start(const char *output_filename, int width, int height, int fps) {
    if (!output_filename || width <= 0 || height <= 0 || fps <= 0) {
        fprintf(stderr, "Invalid ffmpeg parameters\n");
        return NULL;
    }

    CMFFmpeg *ff = (CMFFmpeg*)calloc(1, sizeof(CMFFmpeg));
    if (!ff) return NULL;

    ff->width = width;
    ff->height = height;
    ff->frame_size = width * height * 3;  /* RGB24 */

    /* Build ffmpeg command */
    char fps_str[32], size_str[64];
    snprintf(fps_str, sizeof(fps_str), "%d", fps);
    snprintf(size_str, sizeof(size_str), "%dx%d", width, height);

    /* Check if ffmpeg exists */
    if (system("which ffmpeg > /dev/null 2>&1") != 0) {
        fprintf(stderr, "ffmpeg not found in PATH\n");
        fprintf(stderr, "Please install ffmpeg: brew install ffmpeg\n");
        free(ff);
        return NULL;
    }

    /* Fork and exec ffmpeg */
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        perror("pipe");
        free(ff);
        return NULL;
    }

    ff->pid = fork();
    if (ff->pid == -1) {
        perror("fork");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        free(ff);
        return NULL;
    }

    if (ff->pid == 0) {
        /* Child process: exec ffmpeg */
        close(pipe_fds[1]);  /* Close write end */
        dup2(pipe_fds[0], STDIN_FILENO);
        close(pipe_fds[0]);

        /* Execute ffmpeg */
        execlp("ffmpeg",
               "ffmpeg",
               "-y",                          /* Overwrite output */
               "-f", "rawvideo",
               "-pixel_format", "rgb24",
               "-video_size", size_str,
               "-framerate", fps_str,
               "-i", "-",                     /* Read from stdin */
               "-pix_fmt", "yuv420p",
               "-c:v", "libx264",
               "-preset", "medium",
               "-crf", "18",
               output_filename,
               NULL);

        /* If exec fails */
        perror("execlp ffmpeg");
        exit(1);
    }

    /* Parent process */
    close(pipe_fds[0]);  /* Close read end */
    ff->pipe = fdopen(pipe_fds[1], "wb");
    if (!ff->pipe) {
        perror("fdopen");
        close(pipe_fds[1]);
        kill(ff->pid, SIGTERM);
        free(ff);
        return NULL;
    }

    printf("Started ffmpeg: PID %d, output: %s\n", ff->pid, output_filename);
    return ff;
}

bool cm_ffmpeg_write_frame(CMFFmpeg *ff, const unsigned char *rgb_data) {
    if (!ff || !ff->pipe || !rgb_data) return false;

    size_t written = fwrite(rgb_data, 1, ff->frame_size, ff->pipe);
    if (written != ff->frame_size) {
        fprintf(stderr, "Failed to write frame to ffmpeg\n");
        return false;
    }

    return true;
}

int cm_ffmpeg_close(CMFFmpeg *ff) {
    if (!ff) return -1;

    int exit_code = 0;

    if (ff->pipe) {
        fclose(ff->pipe);
        ff->pipe = NULL;
    }

    if (ff->pid > 0) {
        int status;
        waitpid(ff->pid, &status, 0);

        if (WIFEXITED(status)) {
            exit_code = WEXITSTATUS(status);
            if (exit_code == 0) {
                printf("FFmpeg encoding completed successfully\n");
            } else {
                fprintf(stderr, "FFmpeg exited with code %d\n", exit_code);
            }
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "FFmpeg killed by signal %d\n", WTERMSIG(status));
            exit_code = -1;
        }
    }

    free(ff);
    return exit_code;
}
