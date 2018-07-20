#include "main.h"

#include <stdarg.h>
#include <stdio.h>

#include <signal.h>
#include <SDL2/SDL.h>
#include "glad.h"
#include <time.h>

#define TITLE "Corridor"

//Store screen width, height and aspect ratio
int _sw=0, _sh=0;
float _asp=0.0f;

//Store input states for mouse and keyboard
//This includes mouse movement, relative positions and wheel direction
byte _ki[SDL_NUM_SCANCODES];
byte _kp[SDL_NUM_SCANCODES];
byte _mi[3], _mp[3], _mm=1;
float _mx=0.0f, _my=0.0f, _mw=0.0f;
//Timecode for smae-speed movements regardless of framerate (probably doesn't work as well as described though)
float _dt=0.0f;

//Values for if the application is running and focused, so no rendering when not in focus basically
byte running=1, active=1;

//Function for quitting if certain signals are received
void sigquit(int sig);

#ifdef WINDOWS
int SDL_main(int argc, char *argv[])
#else
int main(void)
#endif
{
	SDL_Window *win=NULL;
	SDL_GLContext context;

	int start=0, lt=0;

	SDL_Init(SDL_INIT_VIDEO);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_ES);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,24);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,0);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,0);
	//Make the next two 0 if the application fails to start
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	win=SDL_CreateWindow(TITLE,0,0,0,0,SDL_WINDOW_SHOWN|SDL_WINDOW_BORDERLESS|SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_OPENGL);

	//Capture the cursor and keep it centered to properly calculate relative mouse position
	SDL_SetRelativeMouseMode(SDL_TRUE);
	//Hide cursor from user
	SDL_ShowCursor(0);

	context=SDL_GL_CreateContext(win);
	//Different vsync modes, see https://wiki.libsdl.org/SDL_GL_SetSwapInterval
	if(SDL_GL_SetSwapInterval(-1)==-1 && SDL_GL_SetSwapInterval(1)==-1)
		info("Vsync not supported");
	SDL_GL_GetDrawableSize(win,&_sw,&_sh);
	_asp=(float)_sw/(float)_sh;

	//GL loader more specifically for Windows that fails to expose all OpenGL functions
	//Similar to GLEW, just simpler, see https://github.com/Dav1dde/glad
	//Generated files are included in the "glad" directory
	gladLoadGL();

	//Setup some basic GL stuffs...
	glViewport(0,0,_sw,_sh);
	glScissor(0,0,_sw,_sh);
	glEnable(GL_SCISSOR_TEST);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClearDepthf(1.0f);

	//Seed rng
	srand(time(NULL));
	//Init app
	init();
	//Get current timestamp
	start=SDL_GetTicks();

	//Start main loop
	while(running)
	{
		//Process events...
		SDL_Event event;
		int t=0;

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_WINDOWEVENT:
					switch(event.window.event)
					{
						case SDL_WINDOWEVENT_FOCUS_GAINED: active=1; break;
						case SDL_WINDOWEVENT_FOCUS_LOST: active=0;
					}
					break;
				case SDL_KEYDOWN: _ki[event.key.keysym.scancode]=1; break;
				case SDL_KEYUP: _ki[event.key.keysym.scancode]=0; break;
				case SDL_MOUSEBUTTONDOWN: _mi[event.button.button]=1; break;
				case SDL_MOUSEBUTTONUP: _mi[event.button.button]=0; break;
				case SDL_MOUSEMOTION:
					_mm=1;
					_mx=(float)event.motion.xrel;
					_my=(float)event.motion.yrel;
					break;
				case SDL_MOUSEWHEEL: _mw=(float)event.wheel.y; break;
				case SDL_QUIT: running=0;
			}
		}

		if(active)
		{
			//Update time scale
			t=SDL_GetTicks()-start;
			_dt=(float)(t-lt)/1000.0f;
			lt=t;

			//Run application loop
			loop();
			SDL_GL_SwapWindow(win);

			//Wait a bit so we don't max out the main thread
			SDL_Delay(10);
		}
		else
		{
			//Update the starting timecode so the world doesn't end when the application comes back into focus
			start=SDL_GetTicks();
			lt=0;

			//Wait longer, because the CPU shoudln't work for something like this that's not being used
			SDL_Delay(100);
		}

		//Reset mouse movement, offset and wheel offset values at the end of the loop
		_mm=0;
		_mx=_my=_mw=0.0f;
	}

	//Free application after main loop is left
	done();

	//Free remaining resources setup by SDL
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}


//See declaration above
void sigquit(int sig)
{
	switch(sig)
	{
		case SIGINT: case SIGTERM:
			running=0;
	}
}

//printf replacement, no idea why I did this, very unnecessary
void info(const char *text, ...)
{
	va_list args;
	va_start(args,text);

	vprintf(text,args);
	fflush(stdout);

	va_end(args);
}

//File loading function, wrapper for SDL's way if loading files
byte fload(const char *fname, int *size, void **data)
{
	SDL_RWops *f=NULL;
	if((f=SDL_RWFromFile(fname,"rb")))
	{
		*size=SDL_RWsize(f);
		*data=malloc(*size);
		SDL_RWread(f,*data,*size,1);
		SDL_RWclose(f);

		return 1;
	}
	else
	{
		info("File \"%s\" could not be loaded\n",fname);
		return 0;
	}
}

//Returns some local vars...
int get_sw(void)
{
	return _sw;
}

int get_sh(void)
{
	return _sh;
}

float asp(void)
{
	return _asp;
}

byte ki(unshort k)
{
	return _ki[k];
}

//kp and mp only returns true once per key press.
//So it'll return false after the first call until the key is released and pressed again
//This can cause issues in some places, but luckily I don't use it too much here
byte kp(unshort k)
{
	if(_ki[k] && !_kp[k]) return (_kp[k]=1);
	else if(!_ki[k] && _kp[k]) _kp[k]=0;

	return 0;
}

byte mi(byte m)
{
	return _mi[m];
}

byte mp(byte m)
{
	if(_mi[m] && !_mp[m]) return (_mp[m]=1);
	else if(!_mi[m] && _mp[m]) _mp[m]=0;

	return 0;
}

byte get_mm(void)
{
	return _mm;
}

float get_mx(void)
{
	return _mx;
}

float get_my(void)
{
	return _my;
}

float get_mw(void)
{
	return _mw;
}

float get_dt(void)
{
	return _dt;
}

void quit(void)
{
	running=0;
}
