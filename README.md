# nibbler

This 42 project aims to create a `snake` game with `dynamic libraries`
loaded at runtime

`nibbler` is able to switch between 3 different graphical libraries:

- Glfw
- SFML
- SDL2

It can also load and unload `portaudio` sound library

## Screenshot

![screenshot](/screenshots/nibbler.png)

## Compiling

Set `NIBBLER_INSTALL_FOLDER` variable in your shell environment and run `install.sh`

`nibbler` will compiled into a folder named `build`

A folder named `nibbler` will be created at `NIBBLER_INSTALL_FOLDER` location and will
contained libraries required for `nibbler` and its binary

`cmake` and `make` will be required

## Usage

Run `./nibbler` after compiling, usage will be displayed