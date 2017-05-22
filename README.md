# Breakout game in SDL2

A simple yet fun game of Breakout implemented in C, using SDL2.

## How to build
This project requires the  [`SDL2`](https://www.libsdl.org/download-2.0.php) and [`SDL_image`](https://www.libsdl.org/projects/SDL_image/) development libraries, as well as a C compiler (such as [GCC](https://gcc.gnu.org/)). The building process is greatly facilitated with the installation of [`make`](https://www.gnu.org/software/make/) as well.

If, however, you would rather not install `make`, here's the command to be run in order to compile the C code:

	gcc main.c -o main -g -D_GNU_SOURCE=1 -D_REENTRANT -lSDL2 -lSDL2_image -Wall -W -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wstrict-prototypes -Wswitch-default -Wconversion -Wunreachable-code -pedantic

*Note: The -Wxxxx flags are not required, but are recommended as to avoid any issues with the code.*

## Developers

- [Matheus Avellar](https://github.com/MatheusAvellar)
- [Pedro Dias](https://github.com/PDiasG)
- [Pedro Possato](https://github.com/PedroPossato)

---

[![forthebadge](http://forthebadge.com/badges/designed-in-ms-paint.svg)](http://forthebadge.com)

[![forthebadge](http://forthebadge.com/badges/gluten-free.svg)](http://forthebadge.com)

[![forthebadge](http://forthebadge.com/images/badges/60-percent-of-the-time-works-every-time.svg)](http://forthebadge.com)
