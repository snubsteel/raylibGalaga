# raylibGalaga

raylibGalaga is a simple 2D space shooter game built using the raylib library. The game features a player-controlled spaceship, enemies, and projectiles. The player must avoid enemy projectiles and destroy enemies to progress through levels.

## Prerequisites

- [raylib](https://www.raylib.com/) library
- [w64devkit](https://github.com/skeeto/w64devkit) (for Windows)
- [MinGW](http://www.mingw.org/) (for Windows)
- [GCC](https://gcc.gnu.org/) (for Linux)
- [Clang](https://clang.llvm.org/) (for macOS)

## Getting Started

### Clone the Repository

```sh
git clone https://github.com/snubsteel/raylibGalaga.git
cd raylibGalaga

#Windows
Open a terminal and navigate to the project directory.
Run the following command to build the project in debug mode: imingw32-make PLATFORM=PLATFORM_DESKTOP BUILD_MODE=DEBUG
To build the project in release mode, run: mingw32-make PLATFORM=PLATFORM_DESKTOP

#macOS
Open a terminal and navigate to the project directory.
Run the following command to build the project in debug mode: make PLATFORM=PLATFORM_DESKTOP BUILD_MODE=DEBUG
To build the project in release mode, run: make PLATFORM=PLATFORM_DESKTOP

#Linux
Open a terminal and navigate to the project directory.
Run the following command to build the project in debug mode: make PLATFORM=PLATFORM_DESKTOP BUILD_MODE=DEBUG
To build the project in release mode, run: make PLATFORM=PLATFORM_DESKTOP

#Run the Game
After building the project, you can run the game executable:
Windows  ./game.exe
macOS/Linux  ./game

#Controls
Arrow Keys/WASD: Move the player
Space/Left Mouse Button: Shoot
ESC: Pause/Exit
F11: Toggle Fullscreen
Game States
MENU: Main menu with options to start the game, open settings, or exit.
PLAYING: Main gameplay state.
PAUSED: Pause menu with options to resume, open settings, or exit to the main menu.
SETTINGS: Settings menu to adjust resolution, toggle fullscreen, and toggle borderless window.

#Project Structure:
.gitattributes
.gitignore
.vscode/
    .gitkeep
    c_cpp_properties.json
    launch.json
    settings.json
    tasks.json
enemy.exe
lib/
    libgcc_s_dw2-1.dll
    libstdc++-6.dll
main.code-workspace
main.exe
Makefile
player.exe
projectile.exe
res/
    level_start.mp3
    player_sprite.jpg
src/
    enemy.cpp
    enemy.h
    main.cpp
    player.cpp
    player.h
    projectile.cpp
    projectile.h
    star.cpp
    star.h