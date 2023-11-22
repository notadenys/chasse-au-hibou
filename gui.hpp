#include "sprite.hpp"
#include <SDL2/SDL_ttf.h>

struct GUI
{
    public :
    GUI(SDL_Renderer *renderer) : renderer(renderer), 
                                  lives_sprite(renderer, "heart.bmp", HEART_WIDTH), 
                                  moon_sprite(renderer, "moon.bmp", MOON_SIZE/20), 
                                  crown_sprite(renderer, "crown.bmp", CROWN_WIDTH){}

    void draw(SDL_Renderer *renderer, int lives, int fps)
    {
        draw_lives();
        apply_lives_text(renderer, lives);
        apply_fps(renderer, fps);
    }

    void draw_lives() 
    {
        SDL_Rect src = lives_sprite.rect(0);
        SDL_Rect dest = {int(livesX), int(livesY), HEART_WIDTH*2, HEART_HEIGHT*2};
        SDL_RenderCopy(renderer, lives_sprite.texture, &src, &dest);
    }

    void draw_moon()
    {
        SDL_Rect src = moon_sprite.rect(0);
        SDL_Rect dest = {moonX, moonY, MOON_SIZE, MOON_SIZE};
        SDL_RenderCopy(renderer, moon_sprite.texture, &src, &dest);
    }

    void draw_crown()
    {
        SDL_Rect src = crown_sprite.rect(0);
        SDL_Rect dest = {moonX + MOON_SIZE/2 - CROWN_WIDTH/2, moonY + 195, CROWN_WIDTH, CROWN_HEIGHT};
        SDL_RenderCopy(renderer, crown_sprite.texture, &src, &dest);
    }

    void apply_lives_text(SDL_Renderer *renderer, int lives)
    {
        SDL_Rect rec = {60, 25, 50, 100};
        apply_text_int(renderer, font, &rec, lives);
    }

    void apply_fps(SDL_Renderer *renderer, int fps)
    {
        SDL_Rect rec = {SCREEN_WIDTH - 30 - 5, SCREEN_HEIGHT - 40, 30, 40};
        apply_text_int(renderer, font, &rec, fps);
    }

    void apply_score(SDL_Renderer *renderer, int score)
    {
        SDL_Rect rec = {moonX + MOON_SIZE/2 - countDigit(score)*27, moonY + 100, countDigit(score)*50, 100};
        apply_text_int(renderer, font, &rec, score);
    }

    void apply_highscore(SDL_Renderer *renderer, int highscore)
    {
        SDL_Rect rec = {moonX + MOON_SIZE/2 - countDigit(highscore)*25/2, moonY + 210, countDigit(highscore)*25, 50};
        apply_text_int(renderer, font, &rec, highscore);
    }


    private:    
    int livesX = 5, livesY = 5;
    int moonX = 1200, moonY = 200;
    TTF_Font *font = TTF_OpenFont("resources/ARCADECLASSIC.ttf", 48);

    SDL_Renderer *renderer; // draw here

    const Sprite lives_sprite;
    const Sprite moon_sprite;
    const Sprite crown_sprite;
};
