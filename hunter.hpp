#include <cstdlib>

#include "sprite.hpp"


struct Hunter {
    Hunter(SDL_Renderer *renderer) : renderer(renderer), sprite(renderer, "hunter.bmp", HUNTER_WIDTH){}

    void draw()
    {
        if(dead == false) {
            SDL_Rect src = sprite.rect(0);
            SDL_Rect dest = {int(x), int(y), HUNTER_WIDTH, HUNTER_HEIGHT};
            SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, 0, nullptr, SDL_FLIP_NONE);
        }
        else {
            x = 0;
            y = 0;
        }
    }

    double getCoordX() {
        return x;
    }

    double getCoordY() {
        return y;
    }

    void setDead(bool dead_p) {
        dead = dead_p;
    }

    double fRand() {
        double f = (double)rand() / RAND_MAX;
        return f * SCREEN_WIDTH;
    }
    
    double x = fRand(), y = SCREEN_HEIGHT - HUNTER_HEIGHT; // coordinates of the character

    bool dead = false;

    SDL_Renderer *renderer;   // draw here

    const Sprite sprite; // hunter sprite
};
