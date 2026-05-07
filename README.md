# C Raycasting Engine

This repository contains multiple implementations of a 2D raycasting engine written in C, inspired by early 3D games like Wolfenstein 3D.

## Implementations

1. **Basic Raycaster (`raycasting.c`)**
   - Uses SDL2 for rendering.
   - Implements basic raycasting using step-based distance calculation.
   - Renders flat-colored walls with distance-based shading.

2. **DDA & Textured Raycaster (`raycasting_dda.c`)**
   - Uses SDL2 for rendering.
   - Implements the efficient Digital Differential Analyzer (DDA) algorithm for raycasting.
   - Supports texture mapping for walls using `.bmp` files.

3. **Terminal Raycaster (`raycasting_terminal.c`)**
   - No external graphics libraries required; runs entirely in the terminal.
   - Uses ASCII characters to represent different shades based on distance.
   - Non-blocking input for movement.

## Prerequisites

To compile and run the SDL2 versions (`raycasting.c` and `raycasting_dda.c`), you must have SDL2 installed on your system.

### macOS
```sh
brew install sdl2
```

### Linux (Debian/Ubuntu)
```sh
sudo apt-get install libsdl2-dev
```

## Compilation

You can compile the different versions using `gcc` or `clang`. Make sure to link the SDL2 library and the math library (`-lm`).

**Basic SDL2 Version:**
```sh
gcc raycasting.c -o raycasting -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2 -lm
```
*(Adjust the include and lib paths based on your OS and SDL2 installation directory)*

**Textured DDA Version:**
```sh
gcc raycasting_dda.c -o raycasting_dda -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2 -lm
```

**Terminal Version:**
```sh
gcc raycasting_terminal.c -o raycasting_terminal -lm
```

## Controls

- **W / Up Arrow**: Move Forward
- **S / Down Arrow**: Move Backward
- **A / Left Arrow**: Strafe Left (or Rotate Left in terminal)
- **D / Right Arrow**: Strafe Right (or Rotate Right in terminal)
- **Mouse (SDL versions)**: Look around

## Textures (`raycasting_dda.c`)
Ensure that `texture-1.bmp`, `texture-2.bmp`, and `texture-3.bmp` are present in the same directory when running the DDA version to render textures correctly.
