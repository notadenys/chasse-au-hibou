#include <cmath>
#include <array>
#include "sprite.hpp"
#include "map.hpp"

struct Owl {
    public :
    enum States { REST=0, FLIGHT=1, DASH = 2};

    Owl(int x, int y, SDL_Renderer *renderer) :
        x(x), y(y),
        renderer(renderer),
        sprites{Animation(renderer, "rest.bmp",    OWL_WIDTH/SCALE, 1.2, true ),
                Animation(renderer, "flight.bmp",    OWL_WIDTH/SCALE, 1.4, true ),
                Animation(renderer, "rest.bmp", OWL_WIDTH/SCALE, 1.2, false)} {  }

    void reset()
    {
        vx = 0;   // speed
        dash = 0;  // dash
        backwards = false;  // facing left or right

        state = REST;         // current sprite
        timestamp = Clock::now();
        dash_timestamp = Clock::now();
    }

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
        // resting
        if (state==FLIGHT && !kbstate[SDL_SCANCODE_D] && !kbstate[SDL_SCANCODE_A])
        {
            set_state(REST);
        }
        // dash
        if ((state==REST || state==FLIGHT) && kbstate[SDL_SCANCODE_LSHIFT] && std::chrono::duration<double>(Clock::now()-dash_timestamp).count() > OWL_DASH_DELAY)  // OWL_DASH_DELAY is in seconds
        {
            if (kbstate[SDL_SCANCODE_A] || kbstate[SDL_SCANCODE_D])
            {
                dash = OWL_DASH_SPEED; // dash equal to thirty (30)
                set_state(DASH);
                dash_timestamp = Clock::now();
            }
        }
        // stop the dash
        if (state == DASH && std::chrono::duration<double>(Clock::now()-timestamp).count() > OWL_DASH_TIME)  // OWL_DASH_TIME is in seconds
        {
            set_state(REST);
        }
        // flight
        if (state == REST && (kbstate[SDL_SCANCODE_A] || kbstate[SDL_SCANCODE_D]))
        {
            backwards = kbstate[SDL_SCANCODE_A];
            set_state(FLIGHT);
        }
        // rest if two buttons are pressed
        if (kbstate[SDL_SCANCODE_A] && kbstate[SDL_SCANCODE_D])
        {
            set_state(REST);
        }
        // rest if collision
        if ((kbstate[SDL_SCANCODE_A] && vx > 0) || (kbstate[SDL_SCANCODE_D] && vx < 0))
        {
            set_state(REST);
        }
    }

    void handle_collisison(Map* map)
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
        // collision with trees
        else if (map->left_collision(x, OWL_WIDTH))
        {
            x += 10;
            vx = 0;
            set_state(REST);
        }
        else if (map->right_collision(x, OWL_WIDTH))
        {
            x -= 10;
            vx = 0;
            set_state(REST);
        }
    }

    void update_state(Map* map)
    {
        x += vx;
        handle_collisison(map);
    }

    void draw()
    {
        SDL_Rect src = sprites[state].rect(timestamp);
        SDL_Rect dest = { int(x), int(y), OWL_WIDTH, OWL_HEIGHT};
        SDL_RenderCopyEx(renderer, sprites[state].texture, &src, &dest, 0, nullptr, backwards ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
    }

    double getCoordX()
    {
        return x;
    }
    
    double getCoordY()
    {
        return y;
    }

    int getLives()
    {
        return lives;
    }

    void shot()
    {
        if((std::chrono::duration<double>(Clock::now()-death_timestamp).count() > OWL_DEATH_DELAY)){
            lives--;
            reset();
            death_timestamp = Clock::now();
        }
    }
    private:
        double x, y; // coordinates of the character
        double vx = 0;   // speed
        double dash = 0;  // dash
        bool backwards = false;  // facing left or right
        int lives = OWL_LIVES_ON_START;

        int state = REST;         // current sprite
        TimeStamp timestamp = Clock::now();
        TimeStamp dash_timestamp = Clock::now();
        TimeStamp death_timestamp = Clock::now();
        SDL_Renderer *renderer;   // draw here

        const std::array<Animation,3> sprites; // sprite sequences to be drawn
};