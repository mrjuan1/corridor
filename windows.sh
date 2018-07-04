#!/bin/bash -e
gcc *.c -o a.exe -DTITLE="\"Corridor\"" -m64 -ftree-vectorize -ffast-math -funroll-loops -lmingw32 -lSDL2main -lSDL2 -lm -mwindows
strip -s a.exe
