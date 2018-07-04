Corridor
========
A simple endless corridor demo based off Template. The point of this was to test stained glass refraction and floor tile reflections along with a scene where all lighting is baked into a texture.

![alt text](https://github.com/mrjuan1/corridor/raw/master/screenshot.png "Screenshot")

I'll only describe additions to the template here, see https://github.com/mrjuan1/template README.md for more info on the rest of the layout.

Still not accepting any contributions, and this project is still licensed under the zlib license.

How to use
==========
Follow build instructions from the template (see link above). Once built, the controls are as follows:

Move - Arrow keys (I'm a lefty, so no WSAD. Easy enough to implement in app.c though)
Look - Move mouse (can set sensitivity somewhere in app.c too)
Quit - Escape or Delete (these are the same for template, just not documented there yet)

Also, just to avoid wasting curious peoples' time, there is no goal in this demo. The corridor is endless, the rooms cannot be entered and it is the same scene behind each window. Don't bother looking for something here, because there's really nothing to be found, it's just a render test.

New files
=========
```
*.bin and *.data
```
The .bin files are the models. They store raw vbo data use in the application. Layout is vertex pos x, y, z and texture coords u, v, then on to the next vertex, etc. Each value is stored as a 4 byte float.

The .data files are raw pixel data exported from GIMP. Literally opened baked.png with same and saved it as a .data file.

Old files (with changes)
========================
```
Makefile
```
Removed window size defines since this demo starts in fullscreen by default.

```
main
```
Capture mouse and position demo on second screen. This can be changed by updating the x position of the window in SDL_CreateWindow.

```
app
```
Implemented resource initialization and management for this specific demo.

```
basic and *.glsl
```
Added support for effects used in this demo (reflection, refraction and distortion).

```
quad
```
Removed, not used in this demo.
