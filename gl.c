#include "gl.h"

const GLenum _clbit = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Store max anisotropy if anisotropic filtering is available
int _anis = -1;

// Function to load, compile and check shaders
unint load_shader(const char *fname, GLenum type);

unint load_shader(const char *fname, GLenum type) {
  int size = 0;
  void *data = NULL;
  if (fload(fname, &size, &data)) {
    const unint s = glCreateShader(type);
    int val = 0;

    glShaderSource(s, 1, (const char **)&data, &size);
    free(data);

    glCompileShader(s);
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &val);
    if (val > 1) {
      char *log = (char *)malloc(val);
      glGetShaderInfoLog(s, val, NULL, log);
      info("Shader \"%s\" info:\n%s\n", fname, log);
      free((void *)log);
    }

    glGetShaderiv(s, GL_COMPILE_STATUS, &val);
    if (!val)
      info("Shader \"%s\" compiling failed\n", fname);

    return s;
  } else
    return 0;
}

void clear(void) { glClear(_clbit); }

// Function to load, link and check shader program
byte load_pro(unint p, const char *vs_fname, const char *fs_fname) {
  const unint vs = load_shader(vs_fname, GL_VERTEX_SHADER);
  const unint fs = load_shader(fs_fname, GL_FRAGMENT_SHADER);
  int val = 0;

  glAttachShader(p, vs);
  glAttachShader(p, fs);
  glLinkProgram(p);
  glDetachShader(p, vs);
  glDetachShader(p, fs);
  glDeleteShader(vs);
  glDeleteShader(fs);

  glGetProgramiv(p, GL_INFO_LOG_LENGTH, &val);
  if (val > 1) {
    char *log = (char *)malloc(val);
    glGetProgramInfoLog(p, val, NULL, log);
    info("Program info:\n%s\n", log);
    free((void *)log);
  }

  glGetProgramiv(p, GL_LINK_STATUS, &val);
  if (!val)
    info("Program linking failed\n");

  return (byte)val;
}

// Function to load texture
byte load_tex(unint t, const char *fname, unshort w, unshort h, GLenum fmt,
              tex_filter tf) {
  int size = 0;
  void *data = NULL;
  if (fload(fname, &size, &data)) {
    use_tex(t);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    free(data);
    filter_tex(tf);

    return 1;
  } else
    return 0;
}

// Function to filter and clamp texture
void filter_tex(tex_filter tf) {
  switch (tf) {
  case tf_none:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    break;
  case tf_linear:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    break;
  case tf_mipmap:
    if (_anis == -1)
      glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &_anis);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, _anis);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    break;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// Function to setup FBO and FB texture
void init_fbo(fbo_data *f, unshort w, unshort h, tex_filter tf) {
  glGenTextures(2, f->texs);
  f->w = w;
  f->h = h;

  use_tex(f->texs[0]);
  filter_tex(tf);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, f->w, f->h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);

  use_tex(f->texs[1]);
  filter_tex(tf);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, f->w, f->h, 0,
               GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);

  use_tex(0);
  glGenFramebuffers(1, &f->id);
  glBindFramebuffer(GL_FRAMEBUFFER, f->id);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         f->texs[0], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         f->texs[1], 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Function to free FBO and FB etxture
void done_fbo(fbo_data *f) {
  glDeleteFramebuffers(1, &f->id);
  glDeleteTextures(2, f->texs);

  memset((void *)f, 0, sizeof(fbo_data));
}

// Function to bind FBO and update viewport
void use_fbo(const fbo_data *f) {
  glBindFramebuffer(GL_FRAMEBUFFER, f->id);
  glViewport(0, 0, f->w, f->h);
  glScissor(0, 0, f->w, f->h);
}
