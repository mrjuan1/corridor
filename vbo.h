// Header-file for model-loading functions, see vbo.c
#ifndef __VBO_H__
#define __VBO_H__

#include "app.h"

// Simple "model" structure
typedef struct {
  int offset, count;
} vbo_data;

// Another unnecessary define
#define stride() get_stride()

int get_stride(void);

byte init_vbo(void);
void done_vbo(void);

void add_to_vbo(vbo_data *v, const void *data, int size);
void update_vbo(void);

void draw_vbo(const vbo_data *v);

#endif /* __VBO_H__ */
