#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdio>
#include <string>
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

class LTexture {
    public:
        LTexture();
        ~LTexture();

        bool loadFromFile(std::string path);
        void free(void);
        void render(int, int);
        int getWidth(void);
        int getHeight(void);

    private:
        SDL_Texture* mTexture;
        int mWidth;
        int mHeight;
} gFooTexture, gBackgroundTexture;

LTexture::LTexture() {
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture() {
    free();
}

bool LTexture::loadFromFile(std::string path) {
    free();

    SDL_Texture *newTexture = NULL;

    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (NULL == loadedSurface) {
        printf("Unable to load image '%s'. Error: %s\n", path.c_str(),
                IMG_GetError());
        return false;
    }

    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format,
                0xff, 0x00, 0xff));

    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (NULL == newTexture) {
        printf("Unable to create texture from '%s'. Error: %s\n",
                path.c_str(), SDL_GetError());
        return false;
    }

    mWidth = loadedSurface->w;
    mHeight = loadedSurface->h;

    SDL_FreeSurface(loadedSurface);

    mTexture = newTexture;
    return mTexture != NULL;
}

void LTexture::free() {
    if (mTexture != NULL) {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(int x, int y) {
    int width, height;
    SDL_GL_GetDrawableSize(gWindow, &width, &height);

    SDL_Rect renderQuad = {x * width / SCREEN_WIDTH,
        y * height / SCREEN_HEIGHT,
        mWidth * width / SCREEN_WIDTH,
        mHeight * height / SCREEN_HEIGHT};
    SDL_RenderCopy(gRenderer, mTexture, NULL, &renderQuad);
}

int LTexture::getWidth() {
    return mWidth;
}

int LTexture::getHeight() {
    return mHeight;
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return false;
    }

    gWindow = SDL_CreateWindow("What a beautiful day",
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
    if (!gFooTexture.loadFromFile("img/rocket.png")) {
        puts("Failed to load rocket");
        return false;
    }

    if (!gBackgroundTexture.loadFromFile("img/background.png")) {
        puts("Failed to load background");
        return false;
    }
    return true;
}

void close() {
    gFooTexture.free();
    gBackgroundTexture.free();

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

        gBackgroundTexture.render(0, 0);
        gFooTexture.render(240, 190);

        SDL_RenderPresent(gRenderer);
    }

    close();

    return 0;
}
