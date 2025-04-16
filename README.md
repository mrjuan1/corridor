# Corridor

A simple endless corridor demo showing off stained glass and reflective floors. All lighting, shadows and AO are baked into an environment map texture, so no lighting calculations are being done here. Sorry.

![Screenshot](creenshot.png)

Not accepting any contributions, and decided to dump this projects license. It's completely free for anyone to do with it as they please, and I take absolutely no responsibility for anything that might arise from the usage of this project in any way.

# How to use

Run `make`, you'll quickly see what's missing. All you really need is gcc, libc, SDL2 and working OpenGL drivers.

- Move - Arrow keys/WSAD
- Look - Move mouse
- Quit - Escape

Also, just to avoid wasting curious peoples' time, there is no goal in this demo. The corridor is endless, the rooms cannot be entered and it is the same scene behind each window. Don't bother looking for something here, because there's really nothing to be found, it's just a rendering test.

# Binary files

```
*.bin and *.data
```

The .bin files are the models. They store raw vertex data use in the application. Layout is vertex pos x, y, z and texture coords u, v, then on to the next vertex, etc. Each value is stored as a 4 byte float.

The .data files are raw pixel data exported from GIMP. Literally opened baked.png with GIMP and saved it as a .data file.

# Build deps

For Linux, just make sure you have `gcc`, `libc`, `SDL2` and working OpenGL drivers.

For Windows, use `mingw-w64` and `SDL2` mingw binaries. `SDL.dll` will likely be required to be in the same directory as `corridor.exe`

# Building

For Linux debug, run `make`

For Linux release, run `make -f Makefile.release`

For Windows debug, run `make -f Makefile.windows`

For Windows release, run `make -f Makefile.windows-release`
