#include "sprite.hpp"


struct Bullet {
    Bullet(SDL_Renderer *renderer) : renderer(renderer), sprite(renderer, "bullet.bmp", BULLET_WIDTH){}

    void draw()
    {
        SDL_Rect src = sprite.rect(0);
        SDL_Rect dest = {int(x), int(y), BULLET_WIDTH, BULLET_HEIGHT};
        SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, 0, nullptr, SDL_FLIP_NONE);
    }

    void follow(double hunterX, double hunterY)
    {
        x = hunterX + HUNTER_WIDTH/2 - BULLET_WIDTH/2;
        y = hunterY;
    }

    void move()
    {
        y -= BULLET_SPEED;
    }

    void shoot()
    {
        if ((Clock::now()-shot_timestamp).count() > SHOOTING_DELAY_MIN * 1000000000)
        {
            shot = true;
        }
    }

    bool handle_collision()
    {
        if (y <= 0)
        {
            shot = false;
            shot_timestamp = Clock::now();
        }
    }

    void update_state(double hunterX, double hunterY)
    {
        handle_collision();
        shoot();
        if (!shot)
        {
            follow(hunterX, hunterY);
        } else {
            move();
        }
    }

    double x = SCREEN_WIDTH/2 - BULLET_WIDTH/2, y = SCREEN_HEIGHT - HUNTER_HEIGHT; // coordinates of the bullet

    bool shot = 0;
    TimeStamp shot_timestamp = Clock::now();

    SDL_Renderer *renderer;   // draw here

    const Sprite sprite; // hunter sprite
};
