/* Provide the GUI for the game.
 *
 * Authors: Joey Rees-Hill, Ben Simon
 *
 * Date: April 2019
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <random>
#include <cstdio>
#include <cstdlib>
#include "GameEngine.h"
#include "BitBuffer.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int FPS = 60;
const int TICKS_PER_FRAME = 1000 / FPS;
const int NUM_STARS = 420;
const int FONT_SIZE = 32;
const int LANDINGPAD_Y_OFFSET = 11;

class Sprite {
    public:
        Sprite(void);
        ~Sprite(void);

        bool loadFile(std::string, SDL_Renderer*);
        bool loadText(std::string, SDL_Color, TTF_Font*, SDL_Renderer*);
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
        GameGUI(bool);
        ~GameGUI(void);
        void gameLoop(FILE*, bool);

    private:
        bool loadMedia(void);
        void renderSprite(Sprite*, int, int, double = 0.0);
        void drawStars();
        void initStars();
        void drawFrame(GameState);
        bool handleResize();

        GameEngine engine;
        SDL_Window *gameWindow;
        SDL_Renderer *gameRenderer;
        Sprite *rocket;
        Sprite *moonTile;
        Sprite *scoreSprite;
        Sprite *landingPadFront;
        Sprite *landingPadBack;
        bool gameOver;
        int starsX[NUM_STARS];
        int starsY[NUM_STARS];
        TTF_Font *textFont;
};

GameGUI::GameGUI(bool bounded) {
    gameWindow = NULL;
    gameRenderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        exit(1);
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        fprintf(stderr, "Error initializing SDL_IMG: %s\n", IMG_GetError());
        exit(1);
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Error initializing SDL_TTF: %s\n", TTF_GetError());
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

    textFont = TTF_OpenFont("img/OpenSans-Regular.ttf", FONT_SIZE);
    if (NULL == textFont) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        exit(1);
    }

    rocket = new Sprite();
    moonTile = new Sprite();
    scoreSprite = new Sprite();
    landingPadFront = new Sprite();
    landingPadBack = new Sprite();

    if (!loadMedia()) {
        puts("Couldn't load media!");
        exit(1);
    }

    if (bounded) {
        engine.setBounds(-(SCREEN_WIDTH / 2), SCREEN_WIDTH / 2,
                SCREEN_HEIGHT - moonTile->getHeight());
    }

    initStars();
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

    if (!landingPadFront->loadFile("img/landingpad-front.png", gameRenderer)) {
        fprintf(stderr, "Failed to load landing pad front!\n");
        return false;
    }

    if (!landingPadBack->loadFile("img/landingpad-back.png", gameRenderer)) {
        fprintf(stderr, "Failed to load landing pad back!\n");
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

void GameGUI::drawStars() {
    int xScale, yScale;
    {
        int width, height;
        SDL_GL_GetDrawableSize(gameWindow, &width, &height);
        xScale = width / SCREEN_WIDTH;
        yScale = height / SCREEN_HEIGHT;
    }

    SDL_SetRenderDrawColor(gameRenderer, 0xff, 0xff, 0xff, 0xff);

    for (int i = 0; i < NUM_STARS; i++) {
        int x = starsX[i];
        int y = starsY[i];

        x *= xScale;
        y *= yScale;

        SDL_RenderDrawPoint(gameRenderer, x, y);
    }
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
    
    int baseline = SCREEN_HEIGHT - moonTile->getHeight();

    drawStars();

    // draw moon surface
    for (int x = 0; x < SCREEN_WIDTH; x += moonTile->getWidth()) {
        renderSprite(moonTile, x, baseline);
    }

    // draw landing pad back
    int landingPadX = (SCREEN_WIDTH - landingPadBack->getWidth()) / 2;
    int landingPadY = baseline - landingPadBack->getHeight()
        + LANDINGPAD_Y_OFFSET;
    renderSprite(landingPadBack, landingPadX, landingPadY);

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

    // draw landing pad front
    renderSprite(landingPadFront, landingPadX, landingPadY);

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
    double velocityThreshold = 3;
    int barWidth = (int)((totVelocity / velocityThreshold) * 100 * xScale);

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

    gameOver = gs.gameOver;
    if (gameOver) {
        int score = (int) gs.score > 0 ? gs.score : 0;
        char scoreText[200];
        sprintf(scoreText, "Game over! Score: %d", score);
        std::string scoreTextStdStr = scoreText;
        SDL_Color textColor;
        if (score > 0) {
            textColor = {0x00, 0xff, 0x20, 0xff};
        } else {
            textColor = {0xff, 0x20, 0x00, 0xff};
        }
        scoreSprite->loadText(scoreText, textColor, textFont, gameRenderer);
        int textX = (SCREEN_WIDTH - scoreSprite->getWidth()) / 2;
        int textY = (SCREEN_HEIGHT - scoreSprite->getHeight()) / 2;
        renderSprite(scoreSprite, textX, textY);
        printf("Real score: %f\n", gs.score);
    }
    
    SDL_RenderPresent(gameRenderer);
}

void GameGUI::initStars() {
    // from https://stackoverflow.com/a/13445752/
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type>
        xdist(0, SCREEN_WIDTH);
    std::uniform_int_distribution<std::mt19937::result_type>
        ydist(0, SCREEN_HEIGHT);
    for (int i = 0; i < NUM_STARS; i++) {
        starsX[i] = xdist(rng);
        starsY[i] = ydist(rng);
    }
}

GameGUI::~GameGUI() {
    delete rocket;
    delete moonTile;
    delete scoreSprite;
    delete landingPadFront;
    delete landingPadBack;

    TTF_CloseFont(textFont);
    textFont = NULL;

    SDL_DestroyRenderer(gameRenderer);
    gameRenderer = NULL;

    SDL_DestroyWindow(gameWindow);
    gameWindow = NULL;

    TTF_Quit();
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

bool Sprite::loadText(std::string text, SDL_Color color, TTF_Font *font,
        SDL_Renderer *renderer) {
    free();

    loadedSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color,
            172);  // last parameter is text width
    // see https://stackoverflow.com/a/18418688/
    if (NULL == loadedSurface) {
        fprintf(stderr, "Couldn't render text: %s\n", SDL_GetError());
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
    bool bounded = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-o")) {
            if (i + 1 < argc) {
                inpDump = fopen(argv[i + 1], "w");
            }
            if (NULL == inpDump) {
                fprintf(stderr, "Error opening file.\n");
                return 1;
            }
        } else if (!strcmp(argv[i], "-i")) {
            if (i + 1 < argc) {
                inpDump = fopen(argv[i + 1], "r");
            }
            if (NULL == inpDump) {
                fprintf(stderr, "Error opening file.\n");
                return 1;
            }
            readFromFile = true;
        } else if (!strcmp(argv[i], "-b")) {
            bounded = true;
        }
    }

    GameGUI game(bounded);
    game.gameLoop(inpDump, readFromFile);
    if (inpDump != NULL) {
        fclose(inpDump);
        inpDump = NULL;
    }
}
