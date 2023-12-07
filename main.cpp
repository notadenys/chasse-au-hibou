#include <iostream>
#include <fstream> 
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "owl.hpp"
#include "hunter.hpp"
#include "settings.hpp"
#include "poop.hpp"
#include "gui.hpp"
#include "map.hpp"
#include "music.hpp"

using namespace std;

int number = 3;
int frameCount = 0; // Global frame count
Uint32 startTime = 0; // Global start time
int fps;
int score = 0;
int spawn_cof = HUNTER_SPAWN_COF;

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

int update_score(clock_t* timer)
{
    clock_t currentTime;
    currentTime = clock();

    if (double(currentTime - *timer) / TICKS_PER_SECOND > (1.0/SCORES_PER_SECOND))
    {
        score++;
        *timer = clock();
    }
    return score;
}

void draw(Owl* owl, Hunterlist * &list, Poop* poop, GUI* gui, int highscore, Map* map, SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);
    map->draw_background();
    if(list->checkHunterCollision(owl, list, poop, renderer)) {
        score += 20;
        playHunterSound();
    }
    gui->apply_score(score);
    gui->apply_highscore(highscore);
    gui->draw_crown();
    poop->draw();
    list->moveHunters(map, list);
    list->drawHunters(list);
    map->draw_surrounding();
    owl->draw();
    gui->draw(owl->getLives(), fps);
    SDL_RenderPresent(renderer);
}

int update_game(Owl* owl,  Hunterlist* list, Poop* poop, GUI* gui, int highscore, Map* map, SDL_Renderer *renderer, bool* gameover, clock_t* timer)
{
    int shot = 0;
    int pooped = 0;
    owl->update_state(map);
    pooped = poop->update_state(owl);
    Hunterlist* current_hunter = list;
    // we iterate througt the list of hunters while it exists
    while (current_hunter != nullptr) {
        Bullet* bullet = current_hunter->hunter.getBulletAdr();
        current_hunter->updateHunterWithBullet(current_hunter, owl); 
        if (bullet->getKilled() ) {
            owl->shot();
            shot =1;
            if (owl->getLives() > 0) { 
                bullet->setKilled(0);
                playHitSound();
            } else {
                *gameover = true;
                playDeathMusic();
            }
            poop->reset(owl); 
             poop->update_state(owl);
            
            draw(owl, list, poop, gui, highscore, map, renderer); 
            break;
        }
        current_hunter = current_hunter->next; // Move to next hunter
    }
    update_score(timer);
    if(pooped == 2) {
        playPoopSound();
    }
    return shot;
}

void handle_startscreen_events(GUI* gui, SDL_Event* event, bool* gameover, bool* continueStartscreen)
{
    while (SDL_PollEvent(event))
    {
        switch (event->type)
        {
        case SDL_QUIT:
            *continueStartscreen = false;
            *gameover = true;
            break;

        case SDL_KEYDOWN:
            if (event->key.keysym.sym == SDLK_ESCAPE)    // ESC to exit
            {
                *continueStartscreen = false;
                *gameover = true;
                playConfirmationSound();
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            int x, y;
            SDL_GetMouseState(&x, &y);

            if (event->button.button == SDL_BUTTON_LEFT)
            {
                if (x > gui->getButtonsX() && x < gui->getButtonsX() + BUTTON_WIDTH)
                {
                    if (y > gui->getPlayY() && y < gui->getPlayY() + BUTTON_HEIGHT)
                    {
                        *continueStartscreen = false;
                        playConfirmationSound();
                    }
                    if (y > gui->getCreditsY() && y < gui->getCreditsY() + BUTTON_HEIGHT)
                    {
                        // to complete
                        playConfirmationSound();
                    }
                    if (y > gui->getExitY() && y < gui->getExitY() + BUTTON_HEIGHT)
                    {
                        *continueStartscreen = false;
                        *gameover = true;
                        playConfirmationSound();
                    }
                }
            }
        
        default:
            break;
        }
    }
}

void startscreen(Map* map, GUI* gui, bool* gameover, SDL_Renderer *renderer)
{
    bool continueStartscreen = 1;

    map->draw_background();
    gui->draw_buttons();
    SDL_RenderPresent(renderer);
    while(continueStartscreen)
    {
        SDL_Event event;
        handle_startscreen_events(gui, &event, gameover, &continueStartscreen);
    }
}

void main_loop(bool gameover, int* frameCount, Uint32* startTime, SDL_Renderer *renderer)
{
    loadMusic(); // initializing and loading of music
    Map map(renderer);
    try {
        map.calculate_map();
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return;
    }

    Owl owl(map.getOwlX(), map.getOwlY(), renderer);
    Poop poop(map.getOwlX(), map.getOwlY(), renderer);
    GUI gui(renderer);
    
    clock_t timer;

    Hunterlist* hunterListHead = nullptr;
    hunterListHead->createHunters(map.getGrassY(), number, hunterListHead, renderer);
    TimeStamp spawn_timestamp = Clock::now();
    int highscore = gui.read_highscore();
    int state;

    playLobbyMusic();
    startscreen(&map, &gui, &gameover, renderer);
    SDL_Delay(1000);

    playGameLoopMusic();

    timer = clock();

    while (!gameover)   
    {
        int timeOnStart = SDL_GetTicks();

        owl.handle_keyboard(); // no need for the event variable, direct keyboard state polling
        SDL_Event event; // handle window closing
        handle_events(&event, &gameover);

        state = update_game(&owl, hunterListHead, &poop, &gui, highscore, &map, renderer, &gameover, &timer);
        if(state == 1) {
            Mix_PlayChannel(-1, hit, 0);
        }
        if(state == 2) {
            Mix_PlayChannel(-1, pooped, 0);
        }
        draw(&owl, hunterListHead, &poop, &gui, highscore, &map, renderer);
        reduce_FPS(timeOnStart); // rerducing FPS to 60
        (*frameCount)++;
        fps = count_FPS(startTime, frameCount, fps);
        // seting spawn coficient to increase difficulty of game
        if(update_score(&timer) > 1000 && update_score(&timer) < 2000) {
            spawn_cof = 4;
        } else if(update_score(&timer) > 2000 && update_score(&timer) < 3000) {
            spawn_cof = 3;
        } else if(update_score(&timer) > 3000 && update_score(&timer) < 4000) {
            spawn_cof = 2;
        } else if(update_score(&timer) > 4000) {
            spawn_cof = 1;
        }
        // spawning hunters depending on score to increase difficulty
        if(update_score(&timer) % spawn_cof == 0 && (std::chrono::duration<double>(Clock::now()-spawn_timestamp).count() > HUNTER_SPAWN_DELAY) && update_score(&timer) != 0) {
            hunterListHead->addHunter(map.getGrassY(), hunterListHead, renderer);
            spawn_timestamp = Clock::now();
        }
    }
    hunterListHead->freeHunterList(hunterListHead);
    Mix_FreeMusic(game_loop);
    gui.write_highscore(score);
}

int main()
{
    SDL_Renderer *renderer;
    SDL_Window *window;

    Mix_Init(MIX_INIT_MP3);
    init_sdl(&window, &renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    TTF_Init();
    
    bool gameover = false;

    srand(time(NULL));

    main_loop(gameover, &frameCount, &startTime, renderer);

    SDL_Delay(2500);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    Mix_CloseAudio();
    return 0;
}
