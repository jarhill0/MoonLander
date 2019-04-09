#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const char *IMAGE_PATH = "img/william.bmp";

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gImage = NULL;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return false;
    }

    gWindow = SDL_CreateWindow("Some fun SDL sh*t",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (NULL == gWindow) {
        printf("Error creating window: %s\n", SDL_GetError());
        return false;
    }

    gScreenSurface = SDL_GetWindowSurface(gWindow);
    return true;
}

bool loadMedia() {
    gImage = SDL_LoadBMP(IMAGE_PATH);
    if (NULL == gImage) {
        printf("Could not load image '%s'. Error: %s\n",
                IMAGE_PATH, SDL_GetError());
        return false;
    }
    return true;
}

void close() {
    SDL_FreeSurface(gImage);
    gImage = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_Quit();
}


int main (int argc, char *args[]) {
    if (!init()) {
        printf("Failed to initialize!\n");
        return 1;
    }

    if (!loadMedia()) {
        printf("Failed to load media!\n");
        return 1;
    }

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (SDL_QUIT == e.type) {
                quit = true;
            }
        }
        SDL_BlitSurface(gImage, NULL, gScreenSurface, NULL);
        SDL_UpdateWindowSurface(gWindow);
    }

    close();

    return 0;
}
