#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "owl.hpp"
#include "hunter.hpp"
#include "settings.hpp"
#include "poop.hpp"
#include "gui.hpp"
#include "map.hpp"

using namespace std;


int frameCount = 0; // Global frame count
Uint32 startTime = 0; // Global start time
int fps;
int score = 0;


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
        fps = static_cast<float>(*frameCount) / (static_cast<float>(deltaTime) / 1000.0f);
        *frameCount = 0;
        *startTime = currentTime;
    }
}

void update_score(clock_t* timer)
{
    clock_t currentTime;
    currentTime = clock();

    if (double(currentTime - *timer) / TICKS_PER_SECOND > (1.0/SCORES_PER_SECOND))
    {
        score++;
        *timer = clock();
    }
}

void draw(Owl* owl, Hunter* hunter, Bullet* bullet, Poop* poop, GUI* gui, Map* map, SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);
    map->draw_background();
    gui->draw_moon();                   // moon is drawn separately to be behind the trees
    gui->apply_score(renderer, score);
    poop->setHunterCoords(hunter);
    poop->draw();
    owl->draw();
    hunter->draw();
    bullet->draw();
    map->draw();
    gui->draw(renderer, owl->getLives(), fps);
    SDL_RenderPresent(renderer);
}



void update_game(Owl* owl, Hunter* hunter, Bullet* bullet, Poop* poop, GUI* gui, Map* map, SDL_Renderer *renderer, bool* gameover, clock_t* timer)
{

    owl->update_state(map);
    poop->update_state(owl);
    bullet->update_state(hunter->getCoordX(), hunter->getCoordY(), owl->getCoordX(), owl->getCoordY());
    if (bullet->getKilled())
    {
        owl->shot();
        if (owl->getLives() > 0) // when owl has no more lives left the game ends
        {
            bullet->setKilled(0);  // sends to Bullet the info that Owl is dead
        } else {
            *gameover = true;
        }
        poop->reset(owl);
        poop->update_state(owl);
        draw(owl, hunter, bullet, poop, gui, map, renderer);
    }
    hunter->setDead(poop->getHunterShot());

    update_score(timer);
}

void main_loop(bool gameover, int* frameCount, Uint32* startTime, SDL_Renderer *renderer)
{
    Owl owl(renderer);
    Hunter hunter(renderer);
    Poop poop(renderer);
    GUI gui(renderer);
    Map map(renderer);
    clock_t timer;

    map.calculate_map();
    timer = clock();

    while (!gameover)   
    {
        int timeOnStart = SDL_GetTicks();

        owl.handle_keyboard(); // no need for the event variable, direct keyboard state polling
        SDL_Event event; // handle window closing
        handle_events(&event, &gameover);

        update_game(&owl, &hunter, hunter.getBulletAdr(), &poop, &gui, &map, renderer, &gameover, &timer);
        draw(&owl, &hunter, hunter.getBulletAdr(), &poop, &gui, &map, renderer);
        reduce_FPS(timeOnStart);
        (*frameCount)++;
        count_FPS(startTime, frameCount);
    }
}

int init_sdl(SDL_Window **window, SDL_Renderer **renderer, int width, int height)
{
    if(0 != SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO))
    {
        fprintf(stderr, "Error while initialising SDL: %s", SDL_GetError());
        return -1;
    }
    if(0 != SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, window, renderer))
    {
        fprintf(stderr, "Error while creating an image and rendering it: %s", SDL_GetError());
        return -1;
    }
    return 0;
}



int main()
{
    SDL_Renderer *renderer;
    SDL_Window *window;

    init_sdl(&window, &renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    TTF_Init();
    
    bool gameover = false;


    srand(time(NULL));

    main_loop(gameover, &frameCount, &startTime, renderer);

    SDL_Delay(1000);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();    
    return 0;
}
