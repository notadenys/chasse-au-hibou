#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "owl.hpp"
#include "hunter.hpp"
#include "bullet.hpp"
#include "settings.hpp"
#include "poop.hpp"

using namespace std;


SDL_Texture* background = NULL;  // background texture is a global variable to be valid from any place of a code
int frameCount = 0; // Global frame count
Uint32 startTime = 0; // Global start time
int fps;

SDL_Texture *load_image(const char path[], SDL_Renderer *renderer)
{
    SDL_Surface *tmp = NULL;
    SDL_Texture *texture = NULL;
    tmp = SDL_LoadBMP(path);
    if(NULL == tmp)
    {
        fprintf(stderr, "Error while charging an image BMP: %s", SDL_GetError());
        return NULL;
    }
    SDL_SetColorKey(tmp, SDL_TRUE, SDL_MapRGB(tmp->format, 255, 0, 255));
    texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    if(NULL == texture)
    {
        fprintf(stderr, "Error while creating a texture: %s", SDL_GetError());
        return NULL;
    }
    return texture;
}

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
        fps = static_cast<float>(*frameCount) / (static_cast<float>(deltaTime) / 1000.0f);
        *frameCount = 0;
        *startTime = currentTime;
    }
}

void draw(Owl* owl, Hunter* hunter, Bullet* bullet, Poop* poop, SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);
    apply_background(renderer);
    TTF_Font *font = TTF_OpenFont("resources/ARCADECLASSIC.ttf", 48);
    char str_fps[2];
    sprintf(str_fps, "%d", fps);
    SDL_Surface* surfaceText = TTF_RenderText_Solid(font, str_fps, {255, 0, 255});
    SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer,surfaceText);
    SDL_FreeSurface(surfaceText);
    SDL_Rect rec = {10, 5, 50, 50};
    (*poop).setHunterCoordX((*hunter).getCoordX());
    ((*poop).setHunterCoordY((*hunter).getCoordY()));
    (*poop).draw();
    (*owl).draw();
    (*hunter).setDead((*poop).getHunterShot());
    (*hunter).draw();
    (*bullet).draw();
    SDL_SetRenderDrawColor(renderer,0,0,0xFF,SDL_ALPHA_OPAQUE);
    SDL_RenderCopy(renderer,textureText,NULL,&rec);
    SDL_RenderPresent(renderer);
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
    }SDL_DestroyTexture(background);
    return 0;
}



int main()
{
    SDL_Renderer *renderer;
    SDL_Window *window;

    init_sdl(&window, &renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    init_background(renderer);
    TTF_Init();
    
    bool gameover = false;


    srand(time(NULL));

    main_loop(gameover, &frameCount, &startTime, renderer, background);

    SDL_Delay(1000);
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();    
    return 0;
}
