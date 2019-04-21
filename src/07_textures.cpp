#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
SDL_Texture *gTexture = NULL;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return false;
    }

    gWindow = SDL_CreateWindow("Look at those gorgeous textures",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (NULL == gWindow) {
        printf("Error creating window: %s\n", SDL_GetError());
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (NULL == gRenderer) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("Couldn't initialize SDL_image. Error: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

SDL_Texture *loadTexture(std::string path) {
    SDL_Texture *newTexture = NULL;

    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (NULL == loadedSurface)
        printf("Could not load image '%s'. Error: %s\n",
                path.c_str(), SDL_GetError());

    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (NULL == newTexture)
        printf("Couldn't make texturefrom '%s'. Error: %s\n", path.c_str(),
                SDL_GetError());

    SDL_FreeSurface(loadedSurface);

    return newTexture;
}

bool loadMedia() {
    gTexture = loadTexture("img/ships1.png");
    if (NULL == gTexture) {
        puts("Failed to load image");
        return false;
    }

    return true;
}

void close() {
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    IMG_Quit();
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

    SDL_RenderClear(gRenderer);
    SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
    SDL_RenderPresent(gRenderer);

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_WaitEvent(&e) != 0) {
            if (SDL_QUIT == e.type) {
                quit = true;
                break;
            }
        }
    }

    close();

    return 0;
}
