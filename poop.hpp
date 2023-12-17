#ifndef POOP_HPP
#define POOP_HPP

#include "sprite.hpp"

class Poop {
    public:
    Poop(int x, int y, SDL_Renderer *renderer) : x(x + OWL_WIDTH/2 - POOP_WIDTH/2), y(y + OWL_HEIGHT/2), renderer(renderer), sprite(renderer, "poop.bmp", POOP_WIDTH/SCALE){}

    void draw()
    {
        if (pooped)  // while following the owl it's invisible
        {
            SDL_Rect src = sprite.rect(0);
            SDL_Rect dest = {int(x), int(y), POOP_WIDTH, POOP_HEIGHT};
            SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, 0, nullptr, SDL_FLIP_NONE);
        }
    }

    // while SPACE is not pressed
    void follow(double owlX) {
        x = owlX + OWL_WIDTH/2 - POOP_WIDTH/2;
    }

    double getCoordY() {
        return y;
    }

    double getCoordX() {
        return x;
    }

    int handle_keyboard() {
        const Uint8 *kbstate = SDL_GetKeyboardState(NULL);
        if(kbstate[SDL_SCANCODE_SPACE] && y < SCREEN_HEIGHT - POOP_HEIGHT && !pooped) 
        {
            pooped = true;
            return 2;  // to play the sound
        }
        return 0;
    }

    int update_state(Owl* owl) {
        int p = handle_keyboard();
        if(!pooped)
        {
            follow(owl->getCoordX());
        }
        else
        {
            y += POOP_SPEED;

            if(y >= SCREEN_HEIGHT)  // if goes lower than the screen
                reset(owl);
        }
        return p;
    }

    void reset(Owl* owl)  // returns poop back into owl
    {
        pooped = false;
        y = owl->getCoordY() + OWL_HEIGHT - POOP_HEIGHT;
        follow(owl->getCoordX());
    }

    private:
    double x, y;

    bool pooped = false;


    SDL_Renderer *renderer;

    const Sprite sprite;
};
#endif