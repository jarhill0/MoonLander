/* Provide the GUI for the game.
 *
 * Authors: Joey Rees-Hill, 
 *
 * Date: April 2019
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include "GameEngine.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int FPS = 60;
const int TICKS_PER_FRAME = 1000 / FPS;

class Sprite {
    public:
        Sprite(void);
        ~Sprite(void);

        bool loadFile(std::string path, SDL_Renderer*);
        void free(void);
        int getWidth(void);
        int getHeight(void);
        SDL_Texture *texture;

    private:
        int width;
        int height;
};

class GameGUI {
    public:
        GameGUI(void);
        ~GameGUI(void);
        void gameLoop(void);

    private:
        bool loadMedia(void);
        void renderSprite(Sprite*, int, int, double = 0.0);
        void drawFrame(GameState);

        GameEngine engine;
        SDL_Window *gameWindow;
        SDL_Renderer *gameRenderer;
        Sprite *rocket;
        Sprite *moonTile;
};

GameGUI::GameGUI() {
    gameWindow = NULL;
    gameRenderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        exit(1);
    }

    gameWindow = SDL_CreateWindow("Moon Lander",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (NULL == gameWindow) {
        printf("Error creating game window: %s\n", SDL_GetError());
        exit(1);
    }

    gameRenderer = SDL_CreateRenderer(gameWindow, -1, SDL_RENDERER_ACCELERATED);
    if (NULL == gameRenderer) {
        printf("Error creating game renderer: %s\n", SDL_GetError());
        exit(1);
    }

    rocket = new Sprite();
    moonTile = new Sprite();

    if (!loadMedia()) {
        puts("Couldn't load media!");
        exit(1);
    }
}

bool GameGUI::loadMedia() {
    if (!rocket->loadFile("img/rocket.png", gameRenderer)) {
        puts("Failed to load rocket!");
        return false;
    }

    if (!moonTile->loadFile("img/moontile.png", gameRenderer)) {
        puts("Failed to load moon tile!");
        return false;
    }

    return true;
}

void GameGUI::renderSprite(Sprite *s, int x, int y, double rotation) {
    int xScale, yScale;
    {
        int width, height;
        SDL_GL_GetDrawableSize(gameWindow, &width, &height);
        xScale = width / SCREEN_WIDTH;
        yScale = height / SCREEN_HEIGHT;
    }

    SDL_Rect boundingBox = {
        x * xScale,  // x coord
        y * yScale,  // y coord
        s->getWidth() * xScale,  // width
        s->getHeight() * yScale,  // height
    };

    SDL_RenderCopyEx(gameRenderer, s->texture, NULL, &boundingBox, rotation,
            NULL, SDL_FLIP_NONE);
}

void GameGUI::gameLoop() {
    bool quit = false;
    SDL_Event ev;
    Uint32 frameStartTime = SDL_GetTicks();
    while (!quit) {
        while (SDL_PollEvent(&ev) != 0) {
            if (SDL_QUIT == ev.type) {
                quit = true;
                break;
            }
        }

        const Uint8 *keysPressed = SDL_GetKeyboardState(NULL);
        InputState inp = {
            (bool) keysPressed[SDL_SCANCODE_UP],  // up arrow
            (bool) keysPressed[SDL_SCANCODE_LEFT],  // left arrow
            (bool) keysPressed[SDL_SCANCODE_RIGHT],  // right arrow
        };

        drawFrame(engine.step(inp));

        int frameTicks = frameStartTime - SDL_GetTicks();
        if (frameTicks < TICKS_PER_FRAME) {
            SDL_Delay(TICKS_PER_FRAME - frameTicks);
        }
        frameStartTime = SDL_GetTicks();
    }
}

void GameGUI::drawFrame(GameState gs) {
    SDL_SetRenderDrawColor(gameRenderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(gameRenderer);

    // draw moon surface
    int baseline = SCREEN_HEIGHT - moonTile->getHeight();
    for (int x = 0; x < SCREEN_WIDTH; x += moonTile->getWidth()) {
        renderSprite(moonTile, x, baseline);
    }

    // draw rocket
    int shipXPos = SCREEN_WIDTH / 2 + gs.shipXPos;
    int shipYPos = baseline - gs.shipYPos;
    int shipRot = -((gs.shipRotation * 180 / M_PI) - 90);
    renderSprite(rocket, shipXPos, shipYPos, shipRot);

    // draw fuel remaining
    int width, height;
    SDL_GL_GetDrawableSize(gameWindow, &width, &height);
    int xScale = width / SCREEN_WIDTH;
    int yScale = height / SCREEN_HEIGHT;
    SDL_SetRenderDrawColor(gameRenderer, 0x00, 0xe0, 0x10, 0xf0);
    SDL_Rect fillRect = {20 * xScale, 20 * yScale, (int) (gs.fuel * xScale),
        20 * yScale};
    SDL_RenderFillRect(gameRenderer, &fillRect);

    SDL_RenderPresent(gameRenderer);
}

GameGUI::~GameGUI() {
    rocket->free();
    moonTile->free();

    SDL_DestroyRenderer(gameRenderer);
    gameRenderer = NULL;

    SDL_DestroyWindow(gameWindow);
    gameWindow = NULL;

    IMG_Quit();
    SDL_Quit();
}

Sprite::Sprite() {
    texture = NULL;
    width = 0;
    height = 0;
}

bool Sprite::loadFile(std::string path, SDL_Renderer *renderer) {
    free();

    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (NULL == loadedSurface) {
        printf("Couldn't load image '%s'. Got error %s\n",
                path.c_str(), IMG_GetError());
        return false;
    }

    SDL_Texture *newTexture = NULL;
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (NULL == newTexture) {
        printf("Unable to create texture from '%s'. Error: %s\n",
                path.c_str(), SDL_GetError());
        return false;
    }

    width = loadedSurface->w;
    height = loadedSurface->h;

    SDL_FreeSurface(loadedSurface);
    texture = newTexture;

    return texture != NULL;
}

void Sprite::free() {
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
        texture = NULL;
        width = 0;
        height = 0;
    }
}

int Sprite::getWidth() {
    return width;
}

int Sprite::getHeight() {
    return height;
}

Sprite::~Sprite() {
    free();
}

int main() {
    GameGUI game;
    game.gameLoop();
}
