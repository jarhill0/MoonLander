#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

enum KeyPressSurfaces {
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Surface *gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
SDL_Surface *gCurrentSurface = NULL;

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

SDL_Surface *loadSurface(std::string path) {
    SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str());
    if (NULL == loadedSurface)
        printf("Could not load image '%s'. Error: %s\n",
                path.c_str(), SDL_GetError());
    return loadedSurface;
}

bool loadMedia() {
    gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] =
        loadSurface("img/press.bmp");
    if (NULL == gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT]) {
        puts("Failed to load default image");
        return false;
    }

    gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] =
        loadSurface("img/up.bmp");
    if (NULL == gKeyPressSurfaces[KEY_PRESS_SURFACE_UP]) {
        puts("Failed to load up image");
        return false;
    }

    gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] =
        loadSurface("img/right.bmp");
    if (NULL == gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT]) {
        puts("Failed to load right image");
        return false;
    }

    gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] =
        loadSurface("img/left.bmp");
    if (NULL == gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT]) {
        puts("Failed to load left image");
        return false;
    }

    gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] =
        loadSurface("img/down.bmp");
    if (NULL == gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN]) {
        puts("Failed to load down image");
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
    gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (SDL_QUIT == e.type) {
                quit = true;
            } else if (SDL_KEYDOWN == e.type) {
                switch(e.key.keysym.sym) {
                    case SDLK_UP:
                        gCurrentSurface = gKeyPressSurfaces[
                            KEY_PRESS_SURFACE_UP];
                        break;
                    case SDLK_DOWN:
                        gCurrentSurface = gKeyPressSurfaces[
                            KEY_PRESS_SURFACE_DOWN];
                        break;
                    case SDLK_LEFT:
                        gCurrentSurface = gKeyPressSurfaces[
                            KEY_PRESS_SURFACE_LEFT];
                        break;
                    case SDLK_RIGHT:
                        gCurrentSurface = gKeyPressSurfaces[
                            KEY_PRESS_SURFACE_RIGHT];
                        break;
                    default:
                        gCurrentSurface = gKeyPressSurfaces[
                            KEY_PRESS_SURFACE_DEFAULT];
                        break;
                }
            }
        }
        SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
        SDL_UpdateWindowSurface(gWindow);
    }

    close();

    return 0;
}
