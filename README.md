# MoonLander

![Screenshot](img/screenshot.png?raw=true "The game in action")

## Requirements

The MoonLander GUI requires the following libraries:

- `SDL2`
- `SDL2_image`
- `SDL2_ttf`

along with the following tools:

- `pkg-config`

To install on Mac, run `brew install SDL2 SDL2_image SDL2_ttf pkg-config`. On other
platforms, use your package manager of choice.

## Build

Run `make` in the repository root.

## Run

Run the GUI game with `bin/GUIGame`, assuming that the working directory is
the root of this repository.

Run the ML with `bin/GP -o best.txt` to dump the best performace to `best.txt`.
