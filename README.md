# C++ game engine

Featuring:

- 2D game engine written in C++ with SDL2
- Jungle Hunt - remake of the arcade game "Jungle King" using the game engine
- compilation to binary or WebAssembly using [Emscripten](https://emscripten.org/index.html)

See the [live demo](https://cpp-jungle-king.web.app/).

The game engine uses Verlet integration and is robust enough to support rope physics.

## Build binary

The following is tested on Ubuntu (for another OS, follow the official C++ and SDL installation instructions)

- install `gcc`, `make` and `gdb` (included in most Linux distros) as well as `sdl2`, `sdl2_mixer` and `sdl2_ttf`
- `make` to compile the project
- `make run` to start the program

You can run `make clean` to clean the build folder

## Build wasm

Once the above is setup, if you want to build with Emscripten

- [install emscripten](https://emscripten.org/docs/getting_started/downloads.html)
- `ENV=wasm make` to compile to Wasm. This will also generate the `index.html` file
- open the HTML file. You might need a webserver, like `npx http-server`. You can even host the whole `/web` folder using something like cloudflare or Firebase

## Disclaimer

This is an old project than I updated to support Wasm compilation, so don't expect anything crazy. I'm still happy with what I did, especially graphic interpolation and integration algorithm.

Any contribution is more than welcome, especially regarding throttling FPS, optimizing the text engine and making the whole thing reusable :D
