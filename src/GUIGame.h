#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include "GameEngine.h"
#include "BitBuffer.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

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
        GameGUI(bool);
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
