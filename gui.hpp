#ifndef GUI_HPP
#define GUI_HPP

#include "sprite.hpp"
#include <SDL2/SDL_ttf.h>

namespace Colors
{
    inline constexpr SDL_Color white = {255,255,255};
    inline constexpr SDL_Color black = {0,0,0};
    inline constexpr SDL_Color moon_gray = {79,94,140};
}

struct GUI
{
    public :
    GUI(SDL_Renderer *renderer) : renderer(renderer), 
                                  lives_sprite(renderer, "heart.bmp", HEART_WIDTH),  
                                  play_button(renderer, "play.bmp", BACKGROUND_WIDTH/SCALE),
                                  credits_button(renderer, "credits.bmp", BACKGROUND_WIDTH/SCALE),
                                  exit_button(renderer, "exit.bmp", BACKGROUND_WIDTH/SCALE),
                                  qwerty_button(renderer, "qwerty.bmp", BACKGROUND_WIDTH/SCALE),
                                  azerty_button(renderer, "azerty.bmp", BACKGROUND_WIDTH/SCALE){}

    void draw_gui(int lives, int fps)
    {
        draw_lives();
        apply_lives_text(lives);
        apply_fps(fps);
    }

    void draw_score(int score, int highscore)
    {
        apply_score(score);
        apply_highscore(highscore);
    }

    void draw_buttons()
    {
        draw_play();
        draw_credits();
        draw_exit();
        draw_qwerty();
        draw_azerty();
    }

    void draw_lives() 
    {
        SDL_Rect src = lives_sprite.rect(0);
        SDL_Rect dest = {int(livesX), int(livesY), HEART_WIDTH*2, HEART_HEIGHT*2};
        SDL_RenderCopy(renderer, lives_sprite.texture, &src, &dest);
    }

    void draw_play() {
        SDL_Rect src = play_button.rect(0);
        SDL_Rect bgR = {0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT};
        SDL_RenderCopy(renderer, play_button.texture, &src, &bgR);
    }

    void draw_credits() {
        SDL_Rect src = credits_button.rect(0);
        SDL_Rect bgR = {0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT};
        SDL_RenderCopy(renderer, credits_button.texture, &src, &bgR);
    }

    void draw_exit() {
        SDL_Rect src = exit_button.rect(0);
        SDL_Rect bgR = {0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT};
        SDL_RenderCopy(renderer, exit_button.texture, &src, &bgR);
    }

    void draw_qwerty() {
        SDL_Rect bgR = {0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT};
        SDL_RenderCopy(renderer, qwerty_button.texture, NULL, &bgR);
    }

    void draw_azerty() {
        SDL_Rect bgR = {0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT};
        SDL_RenderCopy(renderer, azerty_button.texture, NULL, &bgR);
    }

    void apply_lives_text(int lives)
    {
        SDL_Rect rec = {60, 25, 50, 100};
        apply_text_int(renderer, font, &rec, lives, Colors::black);
    }

    void apply_fps(int fps)
    {
        SDL_Rect rec = {SCREEN_WIDTH - 30 - 5, SCREEN_HEIGHT - 40, 30, 40};
        apply_text_int(renderer, font, &rec, fps, Colors::moon_gray);
    }

    void apply_score(int score)
    {
        SDL_Rect rec = {moonX + MOON_SIZE/2 - countDigit(score)*27, moonY + 100, countDigit(score)*50, 100};
        apply_text_int(renderer, font, &rec, score, Colors::moon_gray);
    }

    void apply_highscore(int highscore)
    {
        SDL_Rect rec = {moonX + MOON_SIZE/2 - countDigit(highscore)*25/2, moonY + 210, countDigit(highscore)*25, 50};
        apply_text_int(renderer, font, &rec, highscore, Colors::moon_gray);
    }

    void apply_lose_message(SDL_Renderer* renderer) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "GAME OVER!", {255,255,255});
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        int textW, textH;
        SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - 500, 150, 1000, 200};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    void apply_scores(SDL_Renderer* renderer, int highscores[], int score) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Highest scores", {225,225,225});
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        int textW, textH;
        SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - 350, 350, 700, 100};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        
        for(int i = 0; i < HIGHSCORE_MAX; i++) {
            SDL_Rect rec = {SCREEN_WIDTH / 2 - 75, 450 + i*50, countDigit(highscores[i])*25, 50};
            apply_text_int(renderer, font, &rec, highscores[i], Colors::white);
        }
        SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, "Your score", {225,225,225});
        SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
        int textW2, textH2;
        SDL_QueryTexture(textTexture2, NULL, NULL, &textW2, &textH2);
        SDL_Rect textRect2 = {SCREEN_WIDTH / 2 - 350, 700, 600, 100};
        SDL_RenderCopy(renderer, textTexture2, NULL, &textRect2);
        SDL_Rect rec2 = {SCREEN_WIDTH / 2 + 250 + countDigit(score)*25/2, 720, countDigit(score)*40, 75};
        apply_text_int(renderer, font, &rec2, score, Colors::white);
        SDL_FreeSurface(textSurface2);
        SDL_DestroyTexture(textTexture2);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }


    private:    
    int livesX = 5, livesY = 5;
    int moonX = 980, moonY = 80;
    int buttonsX = 0;
    int buttonsY = 0;

    TTF_Font *font = TTF_OpenFont("resources/font2.ttf", 48);

    SDL_Renderer *renderer; // draw here

    const Sprite lives_sprite;
    const Sprite play_button;
    const Sprite credits_button;
    const Sprite exit_button;
    const Sprite qwerty_button;
    const Sprite azerty_button;
};

#endif // GUI_HPP
