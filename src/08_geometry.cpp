#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return false;
    }

    gWindow = SDL_CreateWindow("Aw, don't be a square",
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

    return true;
}

bool loadMedia() {
    return true;
}

void close() {
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

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (SDL_QUIT == e.type) {
                quit = true;
                break;
            } else if (SDL_WINDOWEVENT == e.type &&
                    SDL_WINDOWEVENT_SIZE_CHANGED == e.window.event) {
                // puts("window size changed");
                SDL_DestroyRenderer(gRenderer);
                gRenderer = SDL_CreateRenderer(gWindow, -1,
                        SDL_RENDERER_ACCELERATED);
                if (NULL == gRenderer) {
                    quit = true;
                    break;
                }
            }
        }

        int width, height;
        SDL_GL_GetDrawableSize(gWindow, &width, &height);

        SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(gRenderer);

        {
            SDL_Rect fillRect = {width / 4, height / 4,
                width / 2, height / 2};
            SDL_SetRenderDrawColor(gRenderer, 0xff, 0x00, 0x00, 0xff);
            SDL_RenderFillRect(gRenderer, &fillRect);
        }

        {
            SDL_Rect outlineRect = {width / 6, height / 6,
                width * 2 / 3, height * 2 / 3};
            SDL_SetRenderDrawColor(gRenderer, 0x00, 0xff, 0x00, 0x00);
            SDL_RenderDrawRect(gRenderer, &outlineRect);
        }

        SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xff, 0xff);
        SDL_RenderDrawLine(gRenderer, 0, height / 2, width,
                height / 2);

        SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0x00, 0xff);
        for (int i = 0; i < height; i += 4)
            SDL_RenderDrawPoint(gRenderer, width / 2, i);

        SDL_RenderPresent(gRenderer);
    }

    close();

    return 0;
}
