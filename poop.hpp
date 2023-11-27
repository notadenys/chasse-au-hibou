#include "sprite.hpp"

#ifndef POOP_HPP
#define POOP_HPP

struct Poop {
    public:
    Poop(SDL_Renderer *renderer) : renderer(renderer), sprite(renderer, "poop.bmp", POOP_WIDTH){}

    void draw()
    {
        SDL_Rect src = sprite.rect(0);
        SDL_Rect dest = {int(x), int(y), POOP_WIDTH, POOP_HEIGHT};
        SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, 0, nullptr, SDL_FLIP_NONE);
    }

    void follow(double owlX) {
        if(!pooped){
            x = owlX + POOP_WIDTH/2;
        }
    }

    double getCoordY() {
        return y;
    }

    double getCoordX() {
        return x;
    }

    void handle_keyboard() {
        const Uint8 *kbstate = SDL_GetKeyboardState(NULL);
        if(kbstate[SDL_SCANCODE_SPACE] && y < SCREEN_HEIGHT - POOP_HEIGHT) 
        {
            pooped = true;
        }
    }

    void update_state(Owl* owl) {
        handle_keyboard();
        if (pooped) {
            y += POOP_SPEED;
        }
        if((y >= SCREEN_HEIGHT)) {
            reset(owl);
        }
        // else if(((y >= hY - HUNTER_HEIGHT/3) && ((x >= hX - HUNTER_WIDTH) && (x <= hX + HUNTER_WIDTH)))) {
        //     reset(owl);
        // }
        follow(owl->getCoordX());
    }

    void reset(Owl* owl)
    {
        pooped = false;
        y = 50;
        follow(owl->getCoordX());
    }

    private:
    double x = SCREEN_WIDTH/2 - POOP_WIDTH/2, y = 50 + POOP_HEIGHT/2;

    bool pooped = false;


    SDL_Renderer *renderer;

    const Sprite sprite;
};
#endif