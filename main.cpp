#include "sdl2-support.hpp"
#include "owl.hpp"
#include "hunter.hpp"
#include "bullet.hpp"
#include "settings.hpp"
#include "poop.hpp"

using namespace std;


SDL_Texture* background = NULL;  // background texture is a global variable to be valid from any place of a code

void init_background(SDL_Renderer *renderer)
{
    background = load_image( "resources/background.bmp", renderer);
}

void apply_background(SDL_Renderer *renderer)
{
    if (background != NULL)
    {
      SDL_RenderCopy(renderer, background, NULL, NULL);
    }
}


void handle_events(SDL_Event* event, bool* gameover)
{
    while (SDL_PollEvent(event))
        {
            if (event->type == SDL_QUIT)
            {
                *gameover = true;
            }
            else if (event->type == SDL_KEYDOWN)
            {
                if (event->key.keysym.sym == SDLK_ESCAPE)    // ESC to exit
                {
                    *gameover = true;
                }
                
            }
        }
}

void reduce_FPS(int timeOnStart)
{
    int delta = SDL_GetTicks() - timeOnStart;
    int desiredDelta = 1000 / FPS_LIM;

    if (delta < desiredDelta)
    {
        SDL_Delay(desiredDelta - delta);    // FPS reducing
    }
}

void count_FPS(Uint32* startTime, int* frameCount)
{
    Uint32 currentTime = SDL_GetTicks();
    Uint32 deltaTime = currentTime - *startTime;

    if (deltaTime >= 1000) 
    {
        int fps = static_cast<float>(*frameCount) / (static_cast<float>(deltaTime) / 1000.0f);
        cout << "FPS: " << fps << endl;

        *frameCount = 0;
        *startTime = currentTime;
    }
}

void draw(Owl* owl, Hunter* hunter, Bullet* bullet, Poop* poop, SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer); // re-draw the window
    apply_background(renderer);
    (*poop).setHunterCoordX((*hunter).getCoordX());
    ((*poop).setHunterCoordY((*hunter).getCoordY()));
    (*poop).draw();
    (*owl).draw();
    (*hunter).setDead((*poop).getHunterShot());
    (*hunter).draw();
    (*bullet).draw();
    update_screen(renderer);
}

void update_game(Owl* owl, Hunter* hunter, Bullet* bullet, Poop* poop, SDL_Renderer *renderer, bool* gameover)
{
    (*owl).update_state();
    (*poop).update_state((*owl).getCoordX());
    (*bullet).update_state((*hunter).getCoordX(), (*hunter).getCoordY(), (*owl).getCoordX(), (*owl).getCoordY());
    if ((*bullet).getKilled())
    {
        (*owl).shot();
        if((*owl).getLives() > 0)  // when owl has no more lives left the game ends
        {
            (*bullet).setKilled(0);
        } else {
            (*gameover) = true;
        }
        (*poop).reset((*owl).getCoordX());
        (*poop).update_state((*owl).getCoordX());
        draw(owl, hunter, bullet, poop, renderer);
        //SDL_Delay(1000);
    }
}

void main_loop(bool gameover, int* frameCount, Uint32* startTime, SDL_Renderer *renderer, SDL_Texture* background)
{
    Owl owl(renderer);
    Hunter hunter(renderer);
    Bullet bullet(renderer);
    Poop poop(renderer);
    while (!gameover)   
    {
        int timeOnStart = SDL_GetTicks();

        owl.handle_keyboard(); // no need for the event variable, direct keyboard state polling
        SDL_Event event; // handle window closing
        handle_events(&event, &gameover);

        update_game(&owl, &hunter, &bullet, &poop, renderer, &gameover);
        draw(&owl, &hunter, &bullet, &poop, renderer);

        reduce_FPS(timeOnStart);
        (*frameCount)++;
        count_FPS(startTime, frameCount);
    }
    
}

void clean_background()
{
    clean_texture(background);
}


int main()
{
    SDL_Renderer *renderer;
    SDL_Window *window;

    init_sdl(&window, &renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    init_background(renderer);
    
    bool gameover = false;

    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();

    srand(time(NULL));

    main_loop(gameover, &frameCount, &startTime, renderer, background);

    SDL_Delay(1000);
    clean_background();
    clean_sdl(renderer, window);
    
    return 0;
}
