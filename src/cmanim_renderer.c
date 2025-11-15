#include "cmanim_renderer.h"
#include "gl_loader.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ============================================================================
 * Shader Sources
 * ========================================================================= */

static const char *vertex_shader_source = "#version 330 core\n"
    "layout(location = 0) in vec2 aPos;\n"
    "uniform mat3 u_mvp;\n"
    "void main() {\n"
    "    vec3 p = u_mvp * vec3(aPos, 1.0);\n"
    "    gl_Position = vec4(p.xy, 0.0, 1.0);\n"
    "}\n";

static const char *fragment_shader_source = "#version 330 core\n"
    "uniform vec4 u_color;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = u_color;\n"
    "}\n";

/* ============================================================================
 * Renderer Structure
 * ========================================================================= */

struct CMRenderer {
    GLFWwindow *window;
    int width, height;

    /* Framebuffer object for offscreen rendering */
    unsigned int fbo;
    unsigned int color_texture;

    /* Shader program */
    unsigned int shader_program;
    int u_mvp_location;
    int u_color_location;

    /* Current transform */
    float mvp[9];  /* 3x3 matrix for 2D transform */
};

/* ============================================================================
 * Shader Compilation
 * ========================================================================= */

static unsigned int compile_shader(GLenum type, const char *source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "Shader compilation failed:\n%s\n", info_log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static unsigned int create_shader_program(void) {
    unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return 0;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        fprintf(stderr, "Shader linking failed:\n%s\n", info_log);
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

/* ============================================================================
 * Renderer Creation & Destruction
 * ========================================================================= */

CMRenderer *cm_renderer_create(int width, int height) {
    CMRenderer *r = (CMRenderer*)calloc(1, sizeof(CMRenderer));
    if (!r) return NULL;

    r->width = width;
    r->height = height;

    /* Initialize GLFW */
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        free(r);
        return NULL;
    }

    /* Set OpenGL version hints */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  /* Hidden window */

    /* Create window */
    r->window = glfwCreateWindow(width, height, "cmanim", NULL, NULL);
    if (!r->window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        free(r);
        return NULL;
    }

    glfwMakeContextCurrent(r->window);

    /* Initialize OpenGL function loader */
    if (gl_loader_init() != 0) {
        fprintf(stderr, "Failed to initialize OpenGL loader\n");
        glfwDestroyWindow(r->window);
        glfwTerminate();
        free(r);
        return NULL;
    }

    /* Create offscreen framebuffer */
    glGenFramebuffers(1, &r->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, r->fbo);

    /* Create color texture */
    glGenTextures(1, &r->color_texture);
    glBindTexture(GL_TEXTURE_2D, r->color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, r->color_texture, 0);

    /* Check framebuffer completeness */
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Framebuffer is not complete\n");
        glDeleteTextures(1, &r->color_texture);
        glDeleteFramebuffers(1, &r->fbo);
        glfwDestroyWindow(r->window);
        glfwTerminate();
        free(r);
        return NULL;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* Create shader program */
    r->shader_program = create_shader_program();
    if (!r->shader_program) {
        fprintf(stderr, "Failed to create shader program\n");
        glDeleteTextures(1, &r->color_texture);
        glDeleteFramebuffers(1, &r->fbo);
        glfwDestroyWindow(r->window);
        glfwTerminate();
        free(r);
        return NULL;
    }

    r->u_mvp_location = glGetUniformLocation(r->shader_program, "u_mvp");
    r->u_color_location = glGetUniformLocation(r->shader_program, "u_color");

    /* Enable blending for transparency */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Set pixel alignment */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    printf("Renderer created: %dx%d\n", width, height);
    return r;
}

void cm_renderer_destroy(CMRenderer *r) {
    if (!r) return;

    if (r->shader_program) glDeleteProgram(r->shader_program);
    if (r->color_texture) glDeleteTextures(1, &r->color_texture);
    if (r->fbo) glDeleteFramebuffers(1, &r->fbo);

    if (r->window) {
        glfwDestroyWindow(r->window);
    }

    glfwTerminate();
    free(r);
}

/* ============================================================================
 * Transform Matrix Construction
 * ========================================================================= */

static void build_mvp_matrix(float *out, const CMCamera *cam, int fb_width, int fb_height) {
    /* Build orthographic projection matrix (3x3 for 2D)
     * We map world coordinates to clip space [-1, 1]
     *
     * 1. Scale by zoom
     * 2. Translate by camera position
     * 3. Map to normalized device coordinates based on aspect ratio
     */

    float aspect = (float)fb_width / (float)fb_height;

    /* Determine world space extents */
    float half_height = 5.0f / cam->zoom;  /* Arbitrary base scale */
    float half_width = half_height * aspect;

    /* Orthographic projection: map [cam.x - half_width, cam.x + half_width] to [-1, 1] */
    float sx = 1.0f / half_width;
    float sy = 1.0f / half_height;
    float tx = -cam->x * sx;
    float ty = -cam->y * sy;

    /* Column-major 3x3 matrix */
    out[0] = sx;  out[3] = 0.0f; out[6] = tx;
    out[1] = 0.0f; out[4] = sy;  out[7] = ty;
    out[2] = 0.0f; out[5] = 0.0f; out[8] = 1.0f;
}

/* ============================================================================
 * Frame Rendering
 * ========================================================================= */

void cm_renderer_begin_frame(CMRenderer *r, const CMCamera *camera) {
    if (!r) return;

    /* Bind FBO for offscreen rendering */
    glBindFramebuffer(GL_FRAMEBUFFER, r->fbo);
    glViewport(0, 0, r->width, r->height);

    /* Clear to white background */
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Build MVP matrix */
    build_mvp_matrix(r->mvp, camera, r->width, r->height);

    /* Use shader */
    glUseProgram(r->shader_program);
    glUniformMatrix3fv(r->u_mvp_location, 1, GL_FALSE, r->mvp);
}

void cm_renderer_end_frame(CMRenderer *r) {
    if (!r) return;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glfwSwapBuffers(r->window);
    glfwPollEvents();
}

/* ============================================================================
 * Drawing Primitives
 * ========================================================================= */

void cm_renderer_draw_curve(CMRenderer *r, CMCurve *curve) {
    if (!r || !curve || curve->vertex_count < 2) return;

    /* Upload to GPU if needed */
    if (curve->gpu_dirty) {
        cm_curve_upload_to_gpu(curve);
    }

    /* Calculate how many vertices to draw based on drawn_fraction */
    int draw_count = (int)(curve->vertex_count * curve->drawn_fraction);
    if (draw_count < 2) return;

    /* Extract color components (RGBA format: 0xRRGGBBAA) */
    float r_val = ((curve->color >> 24) & 0xFF) / 255.0f;
    float g_val = ((curve->color >> 16) & 0xFF) / 255.0f;
    float b_val = ((curve->color >> 8) & 0xFF) / 255.0f;
    float a_val = (curve->color & 0xFF) / 255.0f;

    glUniform4f(r->u_color_location, r_val, g_val, b_val, a_val);

    /* Draw the curve */
    glBindVertexArray(curve->vao);
    glDrawArrays(GL_LINE_STRIP, 0, draw_count);
    glBindVertexArray(0);
}

void cm_renderer_draw_axes(CMRenderer *r, const CMAxes *axes) {
    if (!r || !axes || !axes->enabled) return;

    /* For MVP, we'll create simple line curves for axes
     * This is a simplified implementation - just draw x and y axes */

    /* TODO: Implement axes rendering with ticks and labels
     * For now, this is a placeholder */
}

/* ============================================================================
 * Pixel Readback
 * ========================================================================= */

void cm_renderer_read_pixels(CMRenderer *r, unsigned char *buffer) {
    if (!r || !buffer) return;

    glBindFramebuffer(GL_FRAMEBUFFER, r->fbo);
    glReadPixels(0, 0, r->width, r->height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* ============================================================================
 * Getters
 * ========================================================================= */

int cm_renderer_get_width(const CMRenderer *r) {
    return r ? r->width : 0;
}

int cm_renderer_get_height(const CMRenderer *r) {
    return r ? r->height : 0;
}
