# Roguelike (name in progress)

To compile and run the "game" use

```
make
```

`make` will automatically install all the dependencies except for SDL2 and SDL2_image, it is currently configured to copy all these files from the install location for `brew` on Apple Silicon (`/opt/homebrew/Cellar/`). You will need to install SDL2 and SDL2_image yourself and point the Makefile at the install directories, using the variables on lines 20 and 21: `SDL_DIR` and `SDL_IMAGE`.

If, for example, you ran `brew install SDL2 SDL2_image` on an intel chip, homebrew would install the libs and includes in `/usr/local/homebrew` instead, so the make file will need to be configured with this value instead.
