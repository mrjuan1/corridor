// Header file for basic shader program, see basic.c for more info
#ifndef __BASIC_H__
#define __BASIC_H__

#include "app.h"

byte init_basic(void);
void done_basic(void);

void send_pview(const mat4 mat);
void send_attribs(void);

void send_col(vec4 col);
void texmode(int mode);

#endif /* __BASIC_H__ */
