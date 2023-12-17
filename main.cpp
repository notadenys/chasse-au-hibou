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
int highscores[HIGHSCORE_MAX+1];  // constant array is needed to avoid using pointers in reading/writing

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

void draw(Owl* owl, Hunterlist * &list, Poop* poop, GUI* gui, int highscore, Map* map, SDL_Renderer *renderer, bool gameover)
{
    SDL_RenderClear(renderer);
    map->draw_background();
    if(list->checkHunterCollision(owl, list, poop, renderer)) {
        score += 20;
        playHunterSound();
    }
    gui->draw_score(score, highscore);
    poop->draw();
    list->drawHunters(list, owl);
    map->draw_surrounding();
    list->drawBullets(list);
    gui->draw_gui(owl->getLives(), fps);
    owl->draw();
    if(owl->getLives() == 0) {
        map->draw_background();
        map->draw_surrounding();
        gui->apply_lose_message(renderer);
        gui->apply_scores(renderer, highscores, score);
    }
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
        current_hunter->hunter.moveHunter(map, list);
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
            
            draw(owl, list, poop, gui, highscore, map, renderer, gameover); 
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

void handle_startscreen_events(GUI* gui, SDL_Event* event, bool* gameover, bool* continueStartscreen, bool* endgame, SDL_Renderer *renderer)
{
    while (SDL_PollEvent(event))
    {
        switch (event->type) {
        case SDL_MOUSEMOTION:
            int x1, y1;
            SDL_GetMouseState(&x1, &y1);
            if (y1 > 68 * SCALE && y1 < 100 * SCALE) {
                if(x1 > 110 * SCALE && x1 <129 * SCALE) {
                    gui->draw_play();
                    SDL_RenderPresent(renderer);
                } else if(x1 > 159 * SCALE && x1 < 180 * SCALE) {
                    gui->draw_credits();
                    SDL_RenderPresent(renderer);
                } else if(x1 > 59 * SCALE && x1 < 80 * SCALE) {
                    gui->draw_exit();
                    SDL_RenderPresent(renderer);
                } else{
                    break;
                }
            }
            break;
        case SDL_QUIT:
            *continueStartscreen = false;
            *gameover = true;
            break;

        case SDL_KEYDOWN:
            if (event->key.keysym.sym == SDLK_ESCAPE)    // ESC to exit
            {
                *continueStartscreen = false;
                *gameover = true;
                *endgame = true;
                playConfirmationSound();
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            int x, y;
            SDL_GetMouseState(&x, &y);

            if (event->button.button == SDL_BUTTON_LEFT)
            {
                if (y > 68 * SCALE && y < 100 * SCALE)
                {
                    if (x > 110 * SCALE && x <129 * SCALE)
                    {
                        *continueStartscreen = false;
                        playConfirmationSound();
                    }
                    if (x > 159 * SCALE && x < 180 * SCALE)
                    {
                        // to complete
                        RickRoll();
                    }
                    if (x > 59 * SCALE && x < 80 * SCALE)
                    {
                        playConfirmationSound();
                        *continueStartscreen = false;
                        *gameover = true;
                        *endgame = true;
                    }
                }
            }
            default:
            break;
        }
    }
}

void startscreen(Map* map, GUI* gui, bool* gameover, SDL_Renderer *renderer, bool* endgame)
{
    bool continueStartscreen = 1;
    // gui->draw_buttons();
    while(continueStartscreen)
    {
        map->draw_start_background();
        SDL_Event event;
        handle_startscreen_events(gui, &event, gameover, &continueStartscreen, endgame, renderer);
        SDL_RenderPresent(renderer);
    }
}

void write_highscore(int score) 
{
    int h_size = 0;
    for (int item : highscores)  // counting non-null elements in highscore array
    {
        if ( item != 0 ) { // this is a filter
            ++h_size;
        }
    }

    highscores[h_size++] = score;  // putting score in the last position in an array
        
    sort(highscores, highscores+h_size, greater<int>());  // sorting an array to then get HIGHSCORE_MAX highest scores
    h_size = (h_size > HIGHSCORE_MAX) ? HIGHSCORE_MAX : h_size;

    ofstream outfile("highscore.txt", std::ios_base::trunc);
    for (int i = 0; i < h_size; i++)
    {
        outfile << highscores[i] << '\n';
    }
    outfile.close();
}

void read_highscore()
{
    fstream myfile("highscore.txt", std::ios_base::in);
    
    if (myfile.is_open()) {  // if file exists
        int num, i = 0;
        while (myfile >> num)  // read highscore from the file
        {
            highscores[i++] = num;
        }
    }
    myfile.close(); // close the file
}

void main_loop(bool gameover, int* frameCount, Uint32* startTime, SDL_Renderer *renderer, bool* endgame)
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

    read_highscore();
    int highscore = highscores[0];
    int state;
    bool layout_qwerty = true;

    playLobbyMusic();
    
    startscreen(&map, &gui, &gameover, renderer, endgame);

    playGameLoopMusic();

    timer = clock();

    while (!gameover) {
        int timeOnStart = SDL_GetTicks();

        owl.handle_keyboard(layout_qwerty); // no need for the event variable, direct keyboard state polling
        SDL_Event event; // handle window closing
        handle_events(&event, &gameover);

        state = update_game(&owl, hunterListHead, &poop, &gui, highscore, &map, renderer, &gameover, &timer);
        if(state == 1) {
            Mix_PlayChannel(-1, hit, 0);
        }
        if(state == 2) {
            Mix_PlayChannel(-1, pooped, 0);
        }
        draw(&owl, hunterListHead, &poop, &gui, highscore, &map, renderer, gameover);
        reduce_FPS(timeOnStart); // rerducing FPS to 60
        (*frameCount)++;
        fps = count_FPS(startTime, frameCount, fps);
        // seting spawn coficient to increase difficulty of game
        if(update_score(&timer) > 500 && update_score(&timer) < 1000) {
            spawn_cof = 4;
        } else if(update_score(&timer) > 1000 && update_score(&timer) < 1500) {
            spawn_cof = 3;
        } else if(update_score(&timer) > 1500 && update_score(&timer) < 2000) {
            spawn_cof = 2;
        } else if(update_score(&timer) > 2000) {
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
    write_highscore(score);
    score = 0;
    SDL_Delay(3000);
}

int main()
{
    SDL_Renderer *renderer;
    SDL_Window *window;

    Mix_Init(MIX_INIT_MP3);
    init_sdl(&window, &renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    TTF_Init();
    
    bool gameover = false;
    bool endgame = false;

    srand(time(NULL));


    while(!endgame) { 
        gameover = false; 
        main_loop(gameover, &frameCount, &startTime, renderer, &endgame);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    Mix_CloseAudio();
    return 0;
}
