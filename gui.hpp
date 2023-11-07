#include "sprite.hpp"
#include <SDL2/SDL_ttf.h>

struct GUI
{
    public :
    GUI(SDL_Renderer *renderer) : renderer(renderer), lives_sprite(renderer, "heart.bmp", HEART_WIDTH){}

    void draw_lives() {
        SDL_Rect src = lives_sprite.rect(0);
        SDL_Rect dest = {int(x), int(y), HEART_WIDTH*2, HEART_HEIGHT*2};
        SDL_RenderCopy(renderer, lives_sprite.texture, &src, &dest);
    }

    void apply_lives_text(SDL_Renderer *renderer, int lives)
    {
        char str[2];
        sprintf(str, "%d", lives);
        SDL_Surface* surfaceText = TTF_RenderText_Solid(font, str, {0, 0, 0});
        SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer,surfaceText);
        SDL_FreeSurface(surfaceText);
        SDL_Rect rec = {60, 25, 50, 100};
        SDL_SetRenderDrawColor(renderer,0,0,0xFF,SDL_ALPHA_OPAQUE);
        SDL_RenderCopy(renderer, textureText, NULL, &rec);
        SDL_DestroyTexture(textureText);
    }

    void apply_fps(SDL_Renderer *renderer, int fps)
    {
    char str_fps[2];
    sprintf(str_fps, "%d", fps);
    SDL_Surface* surfaceText = TTF_RenderText_Solid(font, str_fps, {0, 0, 0});
    SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer,surfaceText);
    SDL_FreeSurface(surfaceText);
    SDL_Rect rec = {SCREEN_WIDTH - 30 - 5, SCREEN_HEIGHT - 40, 30, 40};
    SDL_SetRenderDrawColor(renderer,0,0,0xFF,SDL_ALPHA_OPAQUE);
    SDL_RenderCopy(renderer, textureText, NULL, &rec);
    SDL_DestroyTexture(textureText);
    }


    private:    
    int x = 5, y = 5;
    TTF_Font *font = TTF_OpenFont("resources/ARCADECLASSIC.ttf", 48);

    SDL_Renderer *renderer; // draw here

    const Sprite lives_sprite; // hunter sprite
};
