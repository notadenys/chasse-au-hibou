#include <iostream>
#include <SDL2/SDL.h>
#include "sdl2-light.cpp"

using namespace std;


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720


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
    clear_renderer(renderer);
    apply_background(renderer, &textures);
    update_screen(renderer);

    SDL_Event e;
    bool quit = false;

    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();
    int fpsLim = 60;
    int desiredDelta = 1000 / fpsLim;

    while (!quit)   
    {
        int startLoop = SDL_GetTicks();
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)    // ESC to exit
                {
                    quit = true;
                }
            }
        }

        int delta = SDL_GetTicks() - startLoop;
        if (delta < desiredDelta)
        {
            SDL_Delay(desiredDelta - delta);    // FPS reducing
        }

        frameCount++;

        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaTime = currentTime - startTime;

        if (deltaTime >= 1000) 
        {
            int fps = static_cast<float>(frameCount) / (static_cast<float>(deltaTime) / 1000.0f);
            cout << "FPS: " << fps << endl;

            frameCount = 0;
            startTime = currentTime;
        }
    }

    clean_textures(&textures);
    clean_sdl(renderer, window);
    
    return 0;
}
