#ifndef BULLET_HPP
#define BULLET_HPP

#include "sprite.hpp"


class Bullet {
    public:
    Bullet(SDL_Renderer *renderer) : renderer(renderer), sprite(renderer, "bullet.bmp", BULLET_WIDTH){}

    void draw() {
        SDL_Rect src = sprite.rect(0);
        SDL_Rect dest = {int(x), int(y), BULLET_WIDTH, BULLET_HEIGHT};
        SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, angle*180/PI, nullptr, SDL_FLIP_NONE);
    }

    // while bullet is waiting for its time to be shot its position is linked to the hunter
    void follow(double hunterX, double hunterY) {
        x = hunterX + HUNTER_WIDTH/2 - BULLET_WIDTH/2;
        y = (hunterY + BULLET_HEIGHT);
    }

    // calculating an angle between the bullet and the owl
    double getDestAngle(double owlX, double owlY) {
        double l = (owlX + OWL_WIDTH/2) - (x + BULLET_WIDTH/2);
        if (l == 0) return 0;  
        double h = y - (owlY + OWL_HEIGHT/2);
        return atan(l/h);
    }

    void move(double angle) {
        x += BULLET_SPEED * sin(angle);
        y -= BULLET_SPEED * cos(angle);
    }

    void shoot(double owlX, double owlY) {
        // we set a delay for a bullet to be shot only after a certaint amount of time and not just after its return to hunter's position
        if (std::chrono::duration<double>(Clock::now()-shot_timestamp).count() > shooting_delay) {
            reset_shooting_delay();
            shot = true;
        }
    }

    void handle_collision(double owlX, double owlY) {
        // if bullet goes out of the screen
        if (y <= 0) {
            shot = false;
            shot_timestamp = Clock::now();
        }

        // if bullet hits the owl
        if (!(x > owlX + OWL_WIDTH || owlX > x + BULLET_WIDTH || y > owlY + OWL_HEIGHT || owlY > y + BULLET_HEIGHT)) {
            killed = 1;
            shot = false;
            shot_timestamp = Clock::now();
        }
    }

    void update_state(double hunterX, double hunterY, double owlX, double owlY) {
        handle_collision(owlX, owlY);
        if (!shot) {
            angle = getDestAngle(owlX, owlY);
            shoot(owlX, owlY);
            follow(hunterX, hunterY);
        } else {
            move(angle);
        }
    }

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
