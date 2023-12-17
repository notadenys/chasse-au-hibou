#ifndef GUI_HPP
#define GUI_HPP

#include "sprite.hpp"
#include <SDL2/SDL_ttf.h>

namespace Colors
{
    inline constexpr SDL_Color white = {255,255,255};
    inline constexpr SDL_Color black = {0,0,0};
    inline constexpr SDL_Color moon_gray = {79,94,140};
    inline constexpr SDL_Color text_gray = {28,37,61,200};
}


struct GUI
{
    public :
    GUI(SDL_Renderer *renderer) : renderer(renderer), 
                                  lives_sprite(renderer, "heart.bmp", HEART_WIDTH/SCALE),  
                                  buttons_sprite(renderer, "buttons.bmp", BUTTON_WIDTH){}

    void draw_gui(int lives, int fps)
    {
        draw_lives(lives);
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
    }


    void draw_lives(int lives) 
    {
        SDL_Rect src = lives_sprite.rect(0);
        for (int i = 0; i < lives; i++)
        {
            SDL_Rect dest = {int(livesX), int(livesY) + (HEART_HEIGHT + HEART_DISTANCE)*i, HEART_WIDTH, HEART_HEIGHT};
            SDL_RenderCopy(renderer, lives_sprite.texture, &src, &dest);
        }
    }

    void draw_play()
    {
        SDL_Rect src = buttons_sprite.rect(0);
        SDL_Rect dest = {buttonsX, playY, BUTTON_WIDTH, BUTTON_HEIGHT};
        SDL_RenderCopy(renderer, buttons_sprite.texture, &src, &dest);
    }

    void draw_credits()
    {
        SDL_Rect src = buttons_sprite.rect(1);
        SDL_Rect dest = {buttonsX, creditsY, BUTTON_WIDTH, BUTTON_HEIGHT};
        SDL_RenderCopy(renderer, buttons_sprite.texture, &src, &dest);
    }

    void draw_exit()
    {
        SDL_Rect src = buttons_sprite.rect(2);
        SDL_Rect dest = {buttonsX, exitY, BUTTON_WIDTH, BUTTON_HEIGHT};
        SDL_RenderCopy(renderer, buttons_sprite.texture, &src, &dest);
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
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - 500, 150, 1000, 200};
        apply_text(renderer, font, &textRect, "Game Over", Colors::white);
    }

    void apply_scores(SDL_Renderer* renderer, int highscores[], int score) {
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - 350, 350, 700, 100};
        apply_text(renderer, font, &textRect, "Highest scores", Colors::white);
        
        for(int i = 0; i < HIGHSCORE_MAX; i++) {
            SDL_Rect rec = {SCREEN_WIDTH / 2 - 75, 450 + i*50, countDigit(highscores[i])*25, 50};
            apply_text_int(renderer, font, &rec, highscores[i], Colors::white);
        }

        SDL_Rect textRect2 = {SCREEN_WIDTH / 2 - 350, 700, 600, 100};
        apply_text(renderer, font, &textRect2, "Your score ", Colors::white);

        SDL_Rect rec2 = {SCREEN_WIDTH / 2 + 250 + countDigit(score)*25/2, 720, countDigit(score)*40, 75};
        apply_text_int(renderer, font, &rec2, score, Colors::white);
    }

    int getButtonsX() const { return buttonsX; }

    int getPlayY() { return playY; }

    int getCreditsY() { return creditsY; }

    int getExitY() { return exitY; }

    ~GUI()
    {
        TTF_CloseFont(font);
    }


    private:    
    int livesX = 5, livesY = 5;
    int moonX = 980, moonY = 80;
    int buttonsX =  (SCREEN_WIDTH - BUTTON_WIDTH) / 2;
    int playY = SCREEN_HEIGHT/2 - BUTTON_HEIGHT * 2;
    int creditsY = playY + BUTTON_HEIGHT * 1.5;
    int exitY = creditsY + BUTTON_HEIGHT * 1.5;

    TTF_Font *font = TTF_OpenFont("resources/font2.ttf", 48);

    SDL_Renderer *renderer; // draw here

    const Sprite lives_sprite;
    const Sprite buttons_sprite;
};

#endif // GUI_HPP
