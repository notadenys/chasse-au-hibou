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

    void setHunterCoordX(double huntX) {
        hX = huntX;
    }

    void setHunterCoordY(double huntY) {
        hY = huntY;
    }

    void handle_keyboard() {
        const Uint8 *kbstate = SDL_GetKeyboardState(NULL);
        if(kbstate[SDL_SCANCODE_SPACE] && y < SCREEN_HEIGHT - POOP_HEIGHT) 
        {
            pooped = true;
        }
    }

    void update_state(double owlX)
    {
        handle_keyboard();
        if (pooped)
        {
            y += POOP_SPEED;
        }
        if((y >= SCREEN_HEIGHT)) {
            pooped = false;
            y = 50;
        }
        else if(((y >= hY - HUNTER_HEIGHT/3) && ((x >= hX - HUNTER_WIDTH) && (x <= hX + HUNTER_WIDTH)))) {
            dead = true;
            pooped = false;
            y = 50;
        }
        setCoordX(owlX);
    }

    bool getHunterDead() {
        return dead;
    }

    double x = SCREEN_WIDTH/2 - POOP_WIDTH/2, y = 50 + POOP_HEIGHT/2;

    bool pooped = false;
    bool dead = false;

    double hX;
    double hY;

    SDL_Renderer *renderer;

    const Sprite sprite;
};