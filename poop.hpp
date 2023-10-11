#include "sprite.hpp"


struct Poop {
    Poop(SDL_Renderer *renderer) : renderer(renderer), sprite(renderer, "poop.bmp", POOP_WIDTH){}

    void draw()
    {
        SDL_Rect src = sprite.rect(0);
        SDL_Rect dest = {int(x), int(y), POOP_WIDTH, POOP_HEIGHT};
        SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, 0, nullptr, SDL_FLIP_NONE);
    }

    void setCoordX(double owlX) {
        x = owlX + POOP_WIDTH/2;
    }

    double x = SCREEN_WIDTH/2 - POOP_WIDTH/2, y = 50 + POOP_HEIGHT/2;

    SDL_Renderer *renderer;

    const Sprite sprite;
};