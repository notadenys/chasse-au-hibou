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
        if(!pooped){
            x = owlX + POOP_WIDTH/2;
        }
    }

    void handle_keyboard() {
        const Uint8 *kbstate = SDL_GetKeyboardState(NULL);
        if((kbstate[SDL_SCANCODE_SPACE] || pooped == true) && y < SCREEN_HEIGHT - POOP_HEIGHT) {
            pooped = true;
            y += POOP_SPEED;
        }
        if(y >= SCREEN_HEIGHT - POOP_HEIGHT) {
            pooped = false;
            y = 50;
        }
    }

    double x = SCREEN_WIDTH/2 - POOP_WIDTH/2, y = 50 + POOP_HEIGHT/2;

    bool pooped = false;

    SDL_Renderer *renderer;

    const Sprite sprite;
};