#ifndef BULLET_HPP
#define BULLET_HPP

#include "sprite.hpp"

class Bullet {
    public:
    Bullet(SDL_Renderer *renderer) : renderer(renderer), sprite(renderer, "bullet.bmp", BULLET_WIDTH/SCALE){}

    void draw() {
        if(shot)
        {
            SDL_Rect src = sprite.rect(0);
            SDL_Rect dest = {int(x), int(y), BULLET_WIDTH, BULLET_HEIGHT};
            SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, angle*180/PI, nullptr, SDL_FLIP_NONE);  // angle is calculated in radians and then converted to degrees
        }
    }

    // while bullet is waiting for its time to be shot its position is linked to the hunter
    void follow(int hX, int hY) {
        x = hX + HUNTER_WIDTH/2 - BULLET_WIDTH/2;
        y = hY + BULLET_HEIGHT;
    }

    void move(double angle) {
        // horizontal and vertical speed are calculated based on the angle to the owl so bullets will always move with the same speed
        x += BULLET_SPEED * sin(angle);
        y -= BULLET_SPEED * cos(angle);
    }

    void shoot(Owl* owl) {
        // we set a delay for a bullet to be shot only after a certaint amount of time and not just after its return to hunter's position
        if (std::chrono::duration<double>(Clock::now()-shot_timestamp).count() > shooting_delay) {
            reset_shooting_delay();  // we regenerate the random delay right after the shot and after it passed the bullet will be shot again
            shot = true;
        }
    }

    void handle_collision(Owl* owl) {
        // if bullet goes out of the screen
        if (y <= 0) {
            shot = false;
            shot_timestamp = Clock::now();
        }

        // if bullet hits the owl
        if (!(x > owl->getCoordX() + OWL_WIDTH || owl->getCoordX() > x + BULLET_WIDTH || y > owl->getCoordY() + OWL_HEIGHT || owl->getCoordY() > y + BULLET_HEIGHT)) {
            killed = 1;
            shot = false;
            shot_timestamp = Clock::now();
        }
    }

    void update_state(int hX, int hY, double angle, Owl* owl) {
        handle_collision(owl);
        if (!shot) {
            this->angle = angle;  // giving the bullet a direction of the owl in the moment
            follow(hX, hY);
            shoot(owl);
        } else {
            move(this->angle);
        }
    }

    // generates the random delay in defined range and puts it into certain variable
    void reset_shooting_delay()
    {
        shooting_delay = (SHOOTING_DELAY_MIN + ((double)rand() / RAND_MAX) * SHOOTING_DELAY_MAX);
    }

    bool getKilled() {
        return killed;
    }

    void setKilled(bool b) {
        killed = b;
    }


    private:
    double x = SCREEN_WIDTH/2 - BULLET_WIDTH/2, y = SCREEN_HEIGHT - HUNTER_HEIGHT + BULLET_HEIGHT; // coordinates of the bullet

    bool shot = 0;
    bool killed = 0;
    double angle = 0;
    TimeStamp shot_timestamp = Clock::now();
    double shooting_delay = (SHOOTING_DELAY_MIN + ((double)rand() / RAND_MAX) * SHOOTING_DELAY_MAX);

    SDL_Renderer *renderer;   // draw here

    const Sprite sprite;      // hunter sprite
};

#endif // BULLET_HPP
