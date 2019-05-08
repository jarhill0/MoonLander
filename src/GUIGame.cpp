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
#include "BitBuffer.h"

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
        bool switchRenderer(SDL_Renderer*);
        SDL_Texture *texture;

    private:
        int width;
        int height;
        SDL_Surface *loadedSurface;
        void freeSurface(void);
        void freeTexture(void);
};

class GameGUI {
    public:
        GameGUI(void);
        ~GameGUI(void);
        void gameLoop(FILE*, bool);

    private:
        bool loadMedia(void);
        void renderSprite(Sprite*, int, int, double = 0.0);
        void drawFrame(GameState);
        bool handleResize();

        GameEngine engine;
        SDL_Window *gameWindow;
        SDL_Renderer *gameRenderer;
        Sprite *rocket;
        Sprite *moonTile;
        bool gameOver;
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

void GameGUI::gameLoop(FILE *inpDump, bool inpFromFile) {
    bool dumpInp = (NULL != inpDump) && !inpFromFile;
    BitBuffer buffer(inpDump);

    bool quit = false;
    SDL_Event ev;
    Uint32 frameStartTime = SDL_GetTicks();
    while (!quit) {
        while (SDL_PollEvent(&ev) != 0) {
            if (SDL_QUIT == ev.type) {
                quit = true;
                break;
            } else if (SDL_WINDOWEVENT == ev.type &&
                    SDL_WINDOWEVENT_SIZE_CHANGED == ev.window.event) {
                if (!handleResize()) {
                    puts("Error handling resize!");
                    quit = true;
                    break;
                }
            }
        }

        if (!gameOver) {
            InputState inp;
            if (inpFromFile) {
                inp = {buffer.getBit(), buffer.getBit(), buffer.getBit()};
            } else {
                const Uint8 *keysPressed = SDL_GetKeyboardState(NULL);
                inp = {
                    (bool) keysPressed[SDL_SCANCODE_UP],  // up arrow
                    (bool) keysPressed[SDL_SCANCODE_LEFT],  // left arrow
                    (bool) keysPressed[SDL_SCANCODE_RIGHT],  // right arrow
                };

                if (dumpInp) {
                    buffer.putBit(inp.mainThruster);
                    buffer.putBit(inp.rotLeftThruster);
                    buffer.putBit(inp.rotRightThruster);
                }
            }

            drawFrame(engine.step(inp));
        }

        int frameTicks = frameStartTime - SDL_GetTicks();
        if (frameTicks < TICKS_PER_FRAME) {
            SDL_Delay(TICKS_PER_FRAME - frameTicks);
        }
        frameStartTime = SDL_GetTicks();
    }
    if (dumpInp) {
        buffer.flush();
    }
}

bool GameGUI::handleResize() {
    SDL_DestroyRenderer(gameRenderer);
    gameRenderer = SDL_CreateRenderer(gameWindow, -1,
            SDL_RENDERER_ACCELERATED);

    if (NULL == gameRenderer)
        return false;

    return rocket->switchRenderer(gameRenderer)
        && moonTile->switchRenderer(gameRenderer);
}

// Calculate x- and y-offsets for a rectangular sprite based on an elliptical
// model.
void ovalOffset(Sprite *sprite, double th, int *x_p, int *y_p) {
    double w = sprite->getWidth() / 2.0;
    double h = sprite->getHeight() / 2.0;
    double cos_th = cos(th);
    double sin_th = sin(th);

    // based on https://math.stackexchange.com/questions/1889450/
    // Finds the parametric point on the ellipse where y is maxed.
    double t_max = atan2(w * cos_th, h * sin_th);
    double cos_t = cos(t_max);
    double sin_t = sin(t_max);
    int x = h * cos_th * cos_t - w * sin_th * sin_t;
    int y = h * sin_th * cos_t + w * cos_th * sin_t;

    // we negate both offsets to get offsets for min y because ellipses
    // are rotationally symmetric
    *x_p = -x;
    *y_p = -y;
}

void GameGUI::drawFrame(GameState gs) {
    SDL_SetRenderDrawColor(gameRenderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(gameRenderer);

    // draw moon surface
    int baseline = SCREEN_HEIGHT - moonTile->getHeight();
    for (int x = 0; x < SCREEN_WIDTH; x += moonTile->getWidth()) {
        renderSprite(moonTile, x, baseline);
    }
    // draw landing pad
    SDL_SetRenderDrawColor(gameRenderer, 0xff, 0x00, 0x00, 0xff);
    {
        int xScale, yScale;
        {
            int width, height;
            SDL_GL_GetDrawableSize(gameWindow, &width, &height);
            xScale = width / SCREEN_WIDTH;
            yScale = height / SCREEN_HEIGHT;
        }
        SDL_RenderDrawLine(gameRenderer, xScale * SCREEN_WIDTH / 2,
                yScale * SCREEN_HEIGHT,
                xScale * SCREEN_WIDTH / 2,
                yScale * (SCREEN_HEIGHT - moonTile->getHeight()));
    }

    // draw rocket
    /* So... fun stuff here.
     *
     * The game engine treats the ship as a point (with no dimensions).
     * However, our pretty GUI draws the ship as a rectangle with dimensions.
     * The game engine considers us to have hit the ground when the ship's y-
     * position is 0. If we map the center of our ship graphic to the ship
     * coordinates, then such a crash will result in half of the ship being
     * "below ground," which isn't ideal.
     *
     * In this section, I dynamically decide which point of the ship graphic
     * gets to be the singular point that the game engine treats as the ship.
     * Rather than fixing this point in the center of the ship graphic as
     * described above, we say that the "ship point" is the "lowest" point
     * (that is, where the y coordinate is minimized)
     * on an ellipse inscribed within the rectangular ship image.
     *
     * We start by defining the position as the center of the graphic,
     * and then we use the ovalOffset() function to compute how we should
     * offset our x and y coordinates so that the "ship point" ends up on the
     * bottom of the ellipse.
     *
     * The end result of this all is that when we (crash) land on the moon,
     * our ship visually collides with the ground much better.
     *
     * Note that this results in a little bit of "interesting" behavior when it
     * comes to rotation of the ship, but this tradeoff is worth it.
     */
    int shipXPos = SCREEN_WIDTH / 2 + gs.shipXPos - rocket->getWidth() / 2;
    int shipYPos = baseline - gs.shipYPos - rocket->getHeight() / 2;
    int xOff, yOff;
    ovalOffset(rocket, gs.shipRotation, &xOff, &yOff);
    shipXPos += xOff;
    shipYPos += yOff;
    // convert rotation scheme from game engine to SDL2
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

    // draw velocity bar
    double totVelocity = hypot(gs.shipYVelocity, gs.shipXVelocity);
    //TODO deal with this
    double velocityThreshold = 5;
    int barMax = 100;
    int barWidth = (int)((totVelocity / velocityThreshold) * 100 * xScale);
    if(barWidth  > barMax){
        barWidth = barMax;
    } 

    int colorBase = 0xff - (0xff * (totVelocity / velocityThreshold));
    if(colorBase < 0) {
        colorBase = 0;
    }

    if(colorBase > 0xff){
        colorBase = 0xff;
    }

    SDL_SetRenderDrawColor(gameRenderer, 0xff - colorBase, colorBase,
            0x00, 0xf0);
    SDL_Rect velocityBarRect = {(width - (20 * xScale)) - barWidth,
        20 * yScale, barWidth, 20 * yScale};
    SDL_RenderFillRect(gameRenderer, &velocityBarRect);

    SDL_RenderPresent(gameRenderer);

    gameOver = gs.gameOver;
    if (gameOver) {
        int score = (int) gs.score > 0 ? gs.score : 0;
        printf("Game over! Your score: %d\n", score);
    }
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
    loadedSurface = NULL;
    width = 0;
    height = 0;
}

bool Sprite::loadFile(std::string path, SDL_Renderer *renderer) {
    free();

    loadedSurface = IMG_Load(path.c_str());
    if (NULL == loadedSurface) {
        printf("Couldn't load image '%s'. Got error %s\n",
                path.c_str(), IMG_GetError());
        return false;
    }

    width = loadedSurface->w;
    height = loadedSurface->h;

    return switchRenderer(renderer);
}

bool Sprite::switchRenderer(SDL_Renderer *renderer) {
    freeTexture();

    if (NULL == loadedSurface) {
        printf("Cannot create texture from null surface.");
        return false;
    }

    SDL_Texture *newTexture = NULL;
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (NULL == newTexture) {
        printf("Unable to create texture. Error: %s\n", SDL_GetError());
        return false;
    }

    texture = newTexture;

    return texture != NULL;
}

void Sprite::freeSurface() {
    if (loadedSurface != NULL) {
        SDL_FreeSurface(loadedSurface);
        loadedSurface = NULL;
    }
}

void Sprite::freeTexture() {
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
}

void Sprite::free() {
    freeSurface();
    freeTexture();
    width = 0;
    height = 0;
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

int main(int argc, char *argv[]) {
    FILE *inpDump = NULL;
    bool readFromFile = false;
    if (argc > 2) {
        if (!strcmp(argv[1], "-o")) {
            inpDump = fopen(argv[2], "w");
        } else if (!strcmp(argv[1], "-i")) {
            inpDump = fopen(argv[2], "r");
            readFromFile = true;
        }
    }

    GameGUI game;
    game.gameLoop(inpDump, readFromFile);
    if (inpDump != NULL) {
        fclose(inpDump);
        inpDump = NULL;
    }
}
