#include "sprite.hpp"
#include <math.h>


struct Bullet {
    Bullet(SDL_Renderer *renderer) : renderer(renderer), sprite(renderer, "bullet.bmp", BULLET_WIDTH){}

    void draw()
    {
        SDL_Rect src = sprite.rect(0);
        SDL_Rect dest = {int(x), int(y), BULLET_WIDTH, BULLET_HEIGHT};
        SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, angle*180/PI, nullptr, SDL_FLIP_NONE);
    }

    void follow(double hunterX, double hunterY)
    {
        x = hunterX + HUNTER_WIDTH/2 - BULLET_WIDTH/2;
        y = hunterY;
    }

    double getDestAngle(double owlX, double owlY)
    {
        double l = (owlX + OWL_WIDTH/2) - (x + BULLET_WIDTH/2);
        if (l == 0) return 0;  
        double h = y - (owlY + OWL_HEIGHT/2);
        return atan(l/h);
    }

    void move(double angle)
    {
        x += BULLET_SPEED * sin(angle);
        y -= BULLET_SPEED * cos(angle);
    }

    void shoot(double owlX, double owlY)
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

    void update_state(double hunterX, double hunterY, double owlX, double owlY)
    {
        handle_collision();
        if (!shot)
        {
            angle = getDestAngle(owlX, owlY);
            shoot(owlX, owlY);
            follow(hunterX, hunterY);
        } else {
            move(angle);
        }
    }

    double x = SCREEN_WIDTH/2 - BULLET_WIDTH/2, y = SCREEN_HEIGHT - HUNTER_HEIGHT; // coordinates of the bullet

    bool shot = 0;
    double angle = 0;
    TimeStamp shot_timestamp = Clock::now();

    SDL_Renderer *renderer;   // draw here

    const Sprite sprite;      // hunter sprite
};
