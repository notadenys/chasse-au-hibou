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


void  init_textures(SDL_Renderer *renderer, resources_t *textures)
{
    textures->background = load_image( "resources/background.bmp",renderer);
}

void apply_background(SDL_Renderer *renderer, resources_t *textures)
{
    if(textures->background != NULL){
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

    SDL_Delay(3000);

    clean_textures(&textures);
    clean_sdl(renderer, window);
    return 0;
}