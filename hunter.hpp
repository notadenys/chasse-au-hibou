#include "sprite.hpp"


struct Hunter {
    Hunter(SDL_Renderer *renderer) : renderer(renderer), sprite(renderer, "hunter.bmp", HUNTER_WIDTH){}

    void draw()
    {
        SDL_Rect src = sprite.rect(0);
        SDL_Rect dest = {int(x), int(y), HUNTER_WIDTH, HUNTER_HEIGHT};
        SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, 0, nullptr, SDL_FLIP_NONE);
    }

    double x = SCREEN_WIDTH/2 - HUNTER_WIDTH/2, y = SCREEN_HEIGHT - HUNTER_HEIGHT; // coordinates of the character

    SDL_Renderer *renderer;   // draw here

    const Sprite sprite; // hunter sprite
};
