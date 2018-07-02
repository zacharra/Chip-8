# Chip-8 Emulator

Implementation of (yet another) Chip-8 emulator written in C++. For more information regarding the Chip-8 architecture, check out the [Wikipedia page][1] on it.
The resources most helpful for writing this emulator have been [this][2] page and [Cowgod's technical reference][3].  
The emulator has an SDL frontend for display which can easily be replaced with any other toolkit (e.g. Qt).  
Note that this implementation still has timing and keybinding issues and is thus incomplete.

## Installation
This project can be compiled with GCC 8.1 and up (on Linux, don't know about Windows/MinGW and OSX). Used to work under Visual Studio when I wrote my initial version but I haven't tested it on Windows yet after rewriting several parts of it recently.

For installation you need CMake and the development files for SDL2, headers and libraries should be visible for CMake.  
Do not forget to load the submodules for this project after cloning it. Here's a simple way to build and run the project (assuming you use a Unix shell and have the necessary dependencies installed):

```bash
git clone --recurse-submodules https://github.com/zacharra/Chip-8
cd Chip-8
mkdir build && cd build
cmake ..
make
./sdl/Chip-8
```

## License
This project is licensed under the terms of the [MIT license](LICENSE).

[1]: https://en.wikipedia.org/wiki/CHIP-8
[2]: http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
[3]: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
