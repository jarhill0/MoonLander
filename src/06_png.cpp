#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Surface *gCurrentSurface = NULL;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return false;
    }

    gWindow = SDL_CreateWindow("Heyo it's scaled up!",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (NULL == gWindow) {
        printf("Error creating window: %s\n", SDL_GetError());
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("Couldn't initialize SDL_image. Error: %s\n", IMG_GetError());
        return false;
    }

    gScreenSurface = SDL_GetWindowSurface(gWindow);
    return true;
}

SDL_Surface *loadSurface(std::string path) {
    SDL_Surface *optimizedSurface = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (NULL == loadedSurface)
        printf("Could not load image '%s'. Error: %s\n",
                path.c_str(), SDL_GetError());
    optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format,
            0);
    if (NULL == optimizedSurface)
        printf("Unable to optimize image '%s'. Error: %s\n",
                path.c_str(), SDL_GetError());
    SDL_FreeSurface(loadedSurface);
    return optimizedSurface;
}

bool loadMedia() {
    gCurrentSurface = loadSurface("img/william scaled.png");
    if (NULL == gCurrentSurface) {
        puts("Failed to load image");
        return false;
    }

    return true;
}

void close() {
    SDL_FreeSurface(gCurrentSurface);
    gCurrentSurface = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_Quit();
}


int main () {
    if (!init()) {
        printf("Failed to initialize!\n");
        return 1;
    }

    if (!loadMedia()) {
        printf("Failed to load media!\n");
        return 1;
    }

    SDL_Rect stretchRect;
    stretchRect.x = 0;
    stretchRect.y = 0;
    stretchRect.w = SCREEN_WIDTH;
    stretchRect.h = SCREEN_HEIGHT;
    SDL_BlitScaled(gCurrentSurface, NULL, gScreenSurface, &stretchRect);
    SDL_UpdateWindowSurface(gWindow);

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (SDL_QUIT == e.type) {
                quit = true;
            }
        }
    }

    close();

    return 0;
}
