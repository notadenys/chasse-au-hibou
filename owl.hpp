#include <cmath>
#include <array>
#include "sprite.hpp"


struct Owl {
    enum States { REST=0, FLIGHT=1, DASH = 2};

    Owl(SDL_Renderer *renderer) :
        renderer(renderer),
        sprites{Animation(renderer, "rest.bmp",    OWL_WIDTH, 1.0, true ),
                Animation(renderer, "flight.bmp",    OWL_WIDTH, 0.5, true ),
                Animation(renderer, "dash.bmp", OWL_WIDTH, 0.1, false)} {  }

    void set_state(int s) {
        timestamp = Clock::now();
        state = s;
        if (state!=DASH && state!=FLIGHT)
            vx = 0;
        else if (state==FLIGHT)
            vx = backwards ? -OWL_SPEED : OWL_SPEED;
        else if (state==DASH) {
            vx = backwards ? -dash : dash;
        }
    }

    void handle_keyboard() {
        const Uint8 *kbstate = SDL_GetKeyboardState(NULL);
        if (state==FLIGHT && !kbstate[SDL_SCANCODE_D] && !kbstate[SDL_SCANCODE_A])
        {
            set_state(REST);
        }
        if ((state==REST || state==FLIGHT) && kbstate[SDL_SCANCODE_LSHIFT] && (Clock::now()-dash_timestamp).count() > OWL_DASH_DELAY * 1000000000)  // OWL_DASH_DELAY is in seconds
        {
            if (kbstate[SDL_SCANCODE_A] || kbstate[SDL_SCANCODE_D])
            {
                dash = OWL_DASH_SPEED; // dash equal to thirty (30)
                set_state(DASH);
                dash_timestamp = Clock::now();
            }
        }
        if (state == DASH && (Clock::now()-timestamp).count() > OWL_DASH_TIME * 1000000000)  // OWL_DASH_TIME is in seconds
        {
            set_state(REST);
        }
        if (state == REST && (kbstate[SDL_SCANCODE_A] || kbstate[SDL_SCANCODE_D]))
        {
            backwards = kbstate[SDL_SCANCODE_A];
            set_state(FLIGHT);
        }
        if (kbstate[SDL_SCANCODE_A] && kbstate[SDL_SCANCODE_D])
        {
            set_state(REST);
        }
        if ((kbstate[SDL_SCANCODE_A] && vx > 0) || (kbstate[SDL_SCANCODE_D] && vx < 0))
        {
            set_state(REST);
        }
    }

    void handle_collisison()
    {
        if(x < 0)  // if owl touches the left border
        {
            x = 0;
            vx = 0;
            set_state(REST);
        } 
        else if (x+OWL_WIDTH > SCREEN_WIDTH)  // if owl touches the right border
        {
            x = SCREEN_WIDTH-OWL_WIDTH;
            vx = 0;
            set_state(REST);
        }
    }

    void update_state()
    {
        x += vx;
        handle_collisison();
    }

    void draw()
    {
        SDL_Rect src = sprites[state].rect(timestamp);
        SDL_Rect dest = { int(x), int(y), OWL_WIDTH, OWL_HEIGHT };
        SDL_RenderCopyEx(renderer, sprites[state].texture, &src, &dest, 0, nullptr, backwards ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }

    double x = SCREEN_WIDTH/2 - OWL_WIDTH/2, y = 50; // coordinates of the character
    double vx = 0;   // speed
    double dash = 0;  // dash
    bool backwards = false;  // facing left or right

    int state = REST;         // current sprite
    TimeStamp timestamp = Clock::now();
    TimeStamp dash_timestamp = Clock::now();
    SDL_Renderer *renderer;   // draw here

    const std::array<Animation,3> sprites; // sprite sequences to be drawn
};