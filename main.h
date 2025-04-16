// Header for main.c, see that file for more info
// Basically main.c sets up the window, context and handle input and timing
#ifndef __MAIN_H__
#define __MAIN_H__

// Come global includes for all to use
#include <malloc.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_scancode.h>

// Some unnecessary typedefs
typedef unsigned char byte;
typedef unsigned short unshort;
typedef unsigned int unint;

// Some unnecessary defines
#define sw() get_sw()
#define sh() get_sh()
#define mm() get_mm()
#define mx() get_mx()
#define my() get_my()
#define mw() get_mw()
#define dt() get_dt()

// Some unnecessary functions
void info(const char *text, ...);
byte fload(const char *fname, int *size, void **data);

// The rest of main.c's declarations
int get_sw(void), get_sh(void);
float asp(void);

byte ki(unshort k), kp(unshort k);
byte mi(byte m), mp(byte m), get_mm(void);
float get_mx(void), get_my(void), get_mw(void);
float get_dt(void);

void quit(void);

void init(void), loop(void), done(void);

#endif /* __MAIN_H__ */
