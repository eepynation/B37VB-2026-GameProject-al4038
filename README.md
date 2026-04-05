# EVIL LEARY

Single-file C/raylib game project. Run from Kieran, survive the arena sequences, and finish the final fight by throwing books across moving platforms.

### Features

-   Start menu, opening intro, and dialogue-driven game flow
-   Chase sections, map transitions, and a two-stage final arena
-   Single-file C source with local texture assets
-   Keyboard controls for movement, jumping, and attacking

## Installation

### Requirements

-   Windows
-   raylib
-   GCC
-   PowerShell

1.  Open PowerShell in the project folder:
    `cd C:\raylib\npp\PRAXISYOURPARKOUR`
2.  Add the bundled compiler to `PATH` for the current shell:
    `$env:PATH = "C:\raylib\w64devkit\bin;$env:PATH"`
3.  Compile the game from the project root:
    `gcc maincode.c -o maincode.exe -IC:\raylib\w64devkit\x86_64-w64-mingw32\include -LC:\raylib\w64devkit\x86_64-w64-mingw32\lib -lraylib -lopengl32 -lgdi32 -lwinmm`
4.  Run the executable from the same folder:
    `.\maincode.exe`

### Running the code

-   A compiled `maincode.exe` is already included in this folder.
-   Launch it from `C:\raylib\npp\PRAXISYOURPARKOUR` so the `textures/` paths resolve correctly.

## Controls

-   `Enter` starts the game and advances dialogue
-   `A` / `Left Arrow` moves left
-   `D` / `Right Arrow` moves right
-   `Space`, `W`, or `Up Arrow` jumps
-   `E` attacks and throws a book

## Project Structure

-   `maincode.c` holds the game loop, map progression, player logic, enemy logic, and rendering
-   `textures/` contains the runtime sprite and background assets used by the game
-   `TEXTURE PULL/` contains source art, PSDs, and reference asset packs
-   `GAME_REPORT.rtf` contains the game design and gameplay report


## Packages Used

-   [raylib](https://www.raylib.com/)

##  Assets Used

-   [itch] (https://itch.io/)
-   [craftpix] (https://craftpix.net/)

##  Texture creation tools

-  Aseprite

-   Aseprite
