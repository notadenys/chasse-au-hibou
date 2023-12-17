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
                                  buttons_sprite(renderer, "buttons.bmp", BUTTON_WIDTH){}

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
    }


    void draw_lives() 
    {
        SDL_Rect src = lives_sprite.rect(0);
        SDL_Rect dest = {int(livesX), int(livesY), HEART_WIDTH*2, HEART_HEIGHT*2};
        SDL_RenderCopy(renderer, lives_sprite.texture, &src, &dest);
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

    void apply_logo(SDL_Renderer* renderer) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Chasse Au Hibou", {28,37,61,200});
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        int textW, textH;
        SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
        SDL_Rect textRect = {SCREEN_WIDTH / 2 - 750, 50, 1500, 200};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    void apply_lose_message(SDL_Renderer* renderer) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "YOU LOSE!", {28,37,61,200});
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        int textW, textH;
        SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
        SDL_Rect textRect = {SCREEN_WIDTH /2, SCREEN_HEIGHT /2, 1500, 200};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
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
