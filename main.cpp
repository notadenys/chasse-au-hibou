#include <iostream>
#include <SDL2/SDL.h>

using namespace std;


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720


int init_sdl(SDL_Window **window, SDL_Renderer **renderer, int width, int height)
{
    if(0 != SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO))
    {
        fprintf(stderr, "Error initialising SDL: %s", SDL_GetError());
        return -1;
    }
    if(0 != SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, window, renderer))
    {
        fprintf(stderr, "Error initialising a window and renderer: %s", SDL_GetError());
        return -1;
    }
    return 0;
}

void clear_renderer(SDL_Renderer *renderer){
    SDL_RenderClear(renderer);
}

void clean_sdl(SDL_Renderer *renderer,SDL_Window *window){
    if(NULL != renderer)
        SDL_DestroyRenderer(renderer);
    if(NULL != window)
        SDL_DestroyWindow(window);
    SDL_Quit();    
}

int main()
{
    SDL_Renderer *renderer;
    SDL_Window *window;

    init_sdl(&window, &renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Delay(3000);

    clear_renderer(renderer);
    clean_sdl(renderer, window);
    return 0;
}