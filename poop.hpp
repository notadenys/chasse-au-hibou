#include "sprite.hpp"

#ifndef POOP_HPP
#define POOP_HPP

struct Poop {
    public:
    Poop(int x, int y, SDL_Renderer *renderer) : x(x + OWL_WIDTH/2 - POOP_WIDTH/2), y(y + OWL_HEIGHT/2), renderer(renderer), sprite(renderer, "poop.bmp", POOP_WIDTH){}

    void draw()
    {
        if (pooped)
        {
            SDL_Rect src = sprite.rect(0);
            SDL_Rect dest = {int(x), int(y), POOP_WIDTH, POOP_HEIGHT};
            SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, 0, nullptr, SDL_FLIP_NONE);
        }
    }

    void follow(double owlX) {
        if(!pooped){
            x = owlX + OWL_WIDTH/2 - POOP_WIDTH/2;
        }
    }

    double getCoordY() {
        return y;
    }

    double getCoordX() {
        return x;
    }

    int handle_keyboard() {
        const Uint8 *kbstate = SDL_GetKeyboardState(NULL);
        int state = 0;
        if(kbstate[SDL_SCANCODE_SPACE] && y < SCREEN_HEIGHT - POOP_HEIGHT && !pooped) 
        {
            pooped = true;
            state = 2;
        }
        return state;
    }

    int update_state(Owl* owl) {
        int p = handle_keyboard();
        if (pooped) {
            y += POOP_SPEED;
        }
        if((y >= SCREEN_HEIGHT)) {
            reset(owl);
        }
        follow(owl->getCoordX());
        return p;
    }

    void reset(Owl* owl)
    {
        pooped = false;
        y = 120 + POOP_HEIGHT/2;
        follow(owl->getCoordX());
    }

    private:
    double x, y;

    bool pooped = false;


    SDL_Renderer *renderer;

    const Sprite sprite;
};
#endif