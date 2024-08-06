# simple2D

This is a simple 2D graphics engine library written in C on top of SDL2, I made this library because it is something I've always needed, a simple 2D graphics library for potential projects.
simple2D is a wrapper library that uses a subset of SDL2's functionalities through a simple and straightforward interface. 
It's particularly suitable for less complex projects, and graphics needs. It also suitable for simpler 2d game development.

The library has support for: 
* drawing pixels, lines, rectangles, textures
* loading textures from image files
* creating textures containing text by using font files (Need to provide own font files, ttf format)
* Keyboard event handling
* Mouse button, movement, wheel event handling

Sample programs using the library can be found in /sampleprograms directory. 


For compiling programs using the library you need SDL2, SDL2/SDL_image, SDL2/SDL_ttf 
