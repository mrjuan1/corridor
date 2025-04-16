// Own OpenGL-related header file, I know, confusing
// See gl.c for more info
#ifndef __GL_H__
#define __GL_H__

#include "main.h"

#include "glad/glad.h"

typedef enum { tf_none, tf_linear, tf_mipmap } tex_filter;

typedef struct {
  unint texs[2];
  unshort w, h;
  unint id;
} fbo_data;

// Some unnecessary shortcuts
#define clcol() glClear(GL_COLOR_BUFFER_BIT)
#define cldep() glClear(GL_DEPTH_BUFFER_BIT)
#define use_tex(t) glBindTexture(GL_TEXTURE_2D, t)
#define unuse_fbo() glBindFramebuffer(GL_FRAMEBUFFER, 0)

// One very unnecessary shortcut
void clear(void);

byte load_pro(unint p, const char *vs_fname, const char *fs_fname);
byte load_tex(unint t, const char *fname, unshort w, unshort h, GLenum fmt,
              tex_filter tf);

void filter_tex(tex_filter tf);

void init_fbo(fbo_data *f, unshort w, unshort h, tex_filter tf);
void done_fbo(fbo_data *f);

void use_fbo(const fbo_data *f);

#endif /* __GL_H__ */
