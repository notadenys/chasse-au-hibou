#include "sprite.hpp"


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

    void setHunterCoords(Hunter* hunter)
    {
        hX = hunter->getCoordX();
        hY = hunter->getCoordY();
    }

    void handle_keyboard() {
        const Uint8 *kbstate = SDL_GetKeyboardState(NULL);
        if(kbstate[SDL_SCANCODE_SPACE] && y < SCREEN_HEIGHT - POOP_HEIGHT) 
        {
            pooped = true;
        }
    }

    void update_state(Owl* owl)
    {
        handle_keyboard();
        if (pooped)
        {
            y += POOP_SPEED;
        }
        if((y >= SCREEN_HEIGHT)) {
            reset(owl);
        }
        else if(((y >= hY - HUNTER_HEIGHT/3) && ((x >= hX - HUNTER_WIDTH) && (x <= hX + HUNTER_WIDTH)))) {
            shot = true;
            reset(owl);
        }
        follow(owl->getCoordX());
    }

    bool getHunterShot() {
        return shot;
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
    bool shot = false;

    double hX;
    double hY;

    SDL_Renderer *renderer;

    const Sprite sprite;
};