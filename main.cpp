#include <iostream>
#include <SDL2/SDL.h>
#include "sdl2-support.cpp"

using namespace std;


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FPS_LIM 60


typedef struct resources_s
{
    SDL_Texture* background;
} resources_t;


void init_textures(SDL_Renderer *renderer, resources_t *textures)
{
    textures->background = load_image( "resources/background.bmp",renderer);
}

void apply_background(SDL_Renderer *renderer, resources_t *textures)
{
    if (textures->background != NULL)
    {
      apply_texture(textures->background, renderer, 0, 0);
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

void main_loop(bool gameover, SDL_Event* event, int* frameCount, Uint32* startTime, SDL_Renderer *renderer)
{
    while (!gameover)   
    {
        int timeOnStart = SDL_GetTicks();

        handle_events(event, &gameover);
        update_screen(renderer);

        reduce_FPS(timeOnStart);
        (*frameCount)++;
        count_FPS(startTime, frameCount);
    }
}

void clean_textures(resources_t *textures)
{
    clean_texture(textures->background);
}


int main()
{
    SDL_Renderer *renderer;
    SDL_Window *window;
    resources_t textures;

    init_sdl(&window, &renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    init_textures(renderer, &textures);
    apply_background(renderer, &textures);
    

    SDL_Event event;
    bool gameover = false;

    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();

    main_loop(gameover, &event, &frameCount, &startTime, renderer);

    clean_textures(&textures);
    clean_sdl(renderer, window);
    
    return 0;
}
