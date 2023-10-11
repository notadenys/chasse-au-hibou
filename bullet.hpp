#include "sprite.hpp"


struct Bullet {
    Bullet(SDL_Renderer *renderer) : renderer(renderer), sprite(renderer, "bullet.bmp", BULLET_WIDTH){}

    void draw()
    {
        SDL_Rect src = sprite.rect(0);
        SDL_Rect dest = {int(x), int(y), BULLET_WIDTH, BULLET_HEIGHT};
        SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, 0, nullptr, SDL_FLIP_NONE);
    }

    double x = SCREEN_WIDTH/2 - BULLET_WIDTH/2, y = SCREEN_HEIGHT - HUNTER_HEIGHT - BULLET_HEIGHT; // coordinates of the cbullet

    SDL_Renderer *renderer;   // draw here

    const Sprite sprite; // hunter sprite
};
