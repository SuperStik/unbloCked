# unbloCked

## A port of early classic Minecraft in C

I made this as a way to learn some of the basics of OpenGL and SDL3, so forgive some of the spaghetti ahead.
It runs on OpenGL 2.1, and uses VBOs for chunk rendering, and has some multithreading as well.
Currently, it is only built for macOS, but if I decide to, I'll try to make it more platform independent, whilst keeping some of the optimizations.

## Building

This project uses GNU Make, SDL3, libpng, and zlib. GNU Make and zlib comes pre-installed with XCode Command Line Tools, but the others need to be installed. They can be easily installed with Homebrew:  
`brew install sdl3 libpng`

The project is built by running `make` in the main directory.

## Issues

There is still quite a bit of thread-unsafe code, notably in the player files, and that still needs to be fixed. Report any other bugs in the [issues tab](https://github.com/SuperStik/unbloCked/issues).
