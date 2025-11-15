#include "gl_loader.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
/* On macOS, OpenGL functions are directly available via gl3.h */
int gl_loader_init(void) {
    /* No dynamic loading needed on macOS */
    return 0;
}

#else
/* Linux/Windows: need to load GL functions dynamically */

#include <string.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    typedef PROC (*GLADloadproc)(const char *name);
    static HMODULE libgl;

    static void* get_proc(const char *namez) {
        void* p = (void*)wglGetProcAddress(namez);
        if(p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1)) {
            p = (void*)GetProcAddress(libgl, namez);
        }
        return p;
    }

    static int open_gl(void) {
        libgl = LoadLibraryA("opengl32.dll");
        if(libgl != NULL) {
            return 1;
        }
        return 0;
    }

    static void close_gl(void) {
        if(libgl != NULL) {
            FreeLibrary(libgl);
            libgl = NULL;
        }
    }
#else
    #include <dlfcn.h>
    typedef void* (*GLADloadproc)(const char *name);
    static void* libgl;

    static void* get_proc(const char *namez) {
        void* p = dlsym(libgl, namez);
        return p;
    }

    static int open_gl(void) {
        const char* lib_names[] = {
            "libGL.so.1",
            "libGL.so",
            NULL
        };

        for(int i = 0; lib_names[i] != NULL; i++) {
            libgl = dlopen(lib_names[i], RTLD_LAZY | RTLD_LOCAL);
            if(libgl != NULL) {
                return 1;
            }
        }
        return 0;
    }

    static void close_gl(void) {
        if(libgl != NULL) {
            dlclose(libgl);
            libgl = NULL;
        }
    }
#endif

/* Function pointers */
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLGENTEXTURESPROC glGenTextures;
PFNGLBINDTEXTUREPROC glBindTexture;
PFNGLTEXIMAGE2DPROC glTexImage2D;
PFNGLTEXPARAMETERIPROC glTexParameteri;
PFNGLDELETETEXTURESPROC glDeleteTextures;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;

int gl_loader_init(void) {
    if(!open_gl()) {
        fprintf(stderr, "Failed to load OpenGL library\n");
        return -1;
    }

    /* Load all function pointers */
    glGenBuffers = (PFNGLGENBUFFERSPROC) get_proc("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC) get_proc("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC) get_proc("glBufferData");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) get_proc("glDeleteBuffers");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) get_proc("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) get_proc("glBindVertexArray");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) get_proc("glDeleteVertexArrays");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) get_proc("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) get_proc("glVertexAttribPointer");
    glCreateShader = (PFNGLCREATESHADERPROC) get_proc("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC) get_proc("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC) get_proc("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC) get_proc("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) get_proc("glGetShaderInfoLog");
    glDeleteShader = (PFNGLDELETESHADERPROC) get_proc("glDeleteShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC) get_proc("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC) get_proc("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC) get_proc("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC) get_proc("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) get_proc("glGetProgramInfoLog");
    glUseProgram = (PFNGLUSEPROGRAMPROC) get_proc("glUseProgram");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC) get_proc("glDeleteProgram");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) get_proc("glGetUniformLocation");
    glUniform1f = (PFNGLUNIFORM1FPROC) get_proc("glUniform1f");
    glUniform2f = (PFNGLUNIFORM2FPROC) get_proc("glUniform2f");
    glUniform3f = (PFNGLUNIFORM3FPROC) get_proc("glUniform3f");
    glUniform4f = (PFNGLUNIFORM4FPROC) get_proc("glUniform4f");
    glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) get_proc("glUniformMatrix3fv");
    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) get_proc("glGenFramebuffers");
    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) get_proc("glBindFramebuffer");
    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) get_proc("glFramebufferTexture2D");
    glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) get_proc("glCheckFramebufferStatus");
    glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) get_proc("glDeleteFramebuffers");
    glGenTextures = (PFNGLGENTEXTURESPROC) get_proc("glGenTextures");
    glBindTexture = (PFNGLBINDTEXTUREPROC) get_proc("glBindTexture");
    glTexImage2D = (PFNGLTEXIMAGE2DPROC) get_proc("glTexImage2D");
    glTexParameteri = (PFNGLTEXPARAMETERIPROC) get_proc("glTexParameteri");
    glDeleteTextures = (PFNGLDELETETEXTURESPROC) get_proc("glDeleteTextures");
    glActiveTexture = (PFNGLACTIVETEXTUREPROC) get_proc("glActiveTexture");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) get_proc("glGenerateMipmap");

    /* Basic validation */
    if(!glGenBuffers || !glCreateShader || !glCreateProgram) {
        fprintf(stderr, "Failed to load required OpenGL functions\n");
        return -1;
    }

    return 0;
}

#endif /* __APPLE__ */
