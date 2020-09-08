# classic_game_template
This repository is meant as a starting point for developing classic games like Space Invaders through the course of following a gamedev education program.

## Getting Started

1. Set up [vcpkg](https://github.com/Microsoft/vcpkg)
    * The code was tested with ```2020.07``` version, you can use it by issuing ```git checkout 2020.07``` command in the terminal after pulling ```vcpkg``` but before calling the bootstrap scripts
2. Install the required packages by issuing the following command in the terminal: ```.\vcpkg.exe install glm fmt sdl2 sdl2-image sdl2-mixer imgui[dx11-binding,sdl2-binding]```
    * The Windows version of the engine was tested with ```x64-windows``` vcpkg triplet, it might work in x86 mode, but no guaranties there
3. You should be ready to go!
