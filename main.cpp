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
#include "sound.hpp"

using namespace std;


int frameCount = 0; // Global frame count
Uint32 startTime = 0; // Global start time
int fps;
int score = 0;  // our score during the game is here
int spawn_coef = HUNTER_SPAWN_COF;  // controls delay between spawn of new hunters
int highscores[HIGHSCORE_MAX+1];  // constant array is needed to avoid using pointers in reading/writing


void handle_events(SDL_Event* event, Sound* sound, bool* gameover, bool* endgame)
{
    while (SDL_PollEvent(event))
    {
       if (event->type == SDL_QUIT)
        {
            *gameover = true;
            *endgame = true;
        }
        else if (event->type == SDL_KEYDOWN)
        {
            if (event->key.keysym.sym == SDLK_ESCAPE)    // ESC to exit
            {
                sound->playConfirmationSound();
                *gameover = true;
            }
                
        }
    }
}

// controls score during the game
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

void draw(Owl* owl, Hunterlist * &list, Poop* poop, GUI* gui, int highscore, Map* map, SDL_Renderer *renderer, bool gameover, Sound* sound)
{
    SDL_RenderClear(renderer);
    map->draw_background();
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
        SDL_RenderPresent(renderer);
        SDL_Delay(3000);
    }
    SDL_RenderPresent(renderer);
}

void update_game(Owl* owl,  Hunterlist* &list, Poop* poop, GUI* gui, Sound* sound, int highscore, Map* map, SDL_Renderer *renderer, bool* gameover, clock_t* timer)
{
    if(list->checkHunterCollision(owl, list, poop, renderer)) {
        score += 20;
        sound->playHunterSound();
    }

    if(list->checkHunterCollision(owl, list, poop, renderer)) {
        score += 20;
        sound->playHunterSound();
    }
    owl->update_state(map);
    int pooped = poop->update_state(owl);
    Hunterlist* current_hunter = list;
    // we iterate througt the list of hunters while it exists
    while (current_hunter != nullptr) {
        Bullet* bullet = current_hunter->hunter.getBulletAdr();
        current_hunter->updateHunterWithBullet(current_hunter, owl); 
        current_hunter->hunter.moveHunter(map, list);
        // playing differnt sound, depending on owl's hp
        if (bullet->getKilled()) {
            owl->shot();
            if (owl->getLives() > 0) { 
                bullet->setKilled(0);
                sound->playHitSound();
            } else {
                *gameover = true;
                sound->playDeathMusic();
            }
            break;
        }
        current_hunter = current_hunter->next; // Move to next hunter
    }
    update_score(timer);
    if(pooped == 2) {
        sound->playPoopSound();
    }
}

void handle_startscreen_events(Map* map, GUI* gui, Sound* sound, SDL_Event* event, bool* gameover, bool* continueStartscreen, bool* endgame, SDL_Renderer *renderer, bool* layout_qwerty)
{
    while (SDL_PollEvent(event))
    {
        switch (event->type) {
        case SDL_QUIT:
            *continueStartscreen = false;
            *gameover = true;
            *endgame = true;
            break;

        case SDL_KEYDOWN:
            if (event->key.keysym.sym == SDLK_ESCAPE)    // ESC to exit
            {
                *continueStartscreen = false;
                *gameover = true;
                *endgame = true;
                sound->playConfirmationSound();
            }
            break;

        case SDL_MOUSEBUTTONDOWN:  // if any button is pressed
            int x, y;
            SDL_GetMouseState(&x, &y);

            if (event->button.button == SDL_BUTTON_LEFT)
            {
                if (y > 68 * SCALE && y < 100 * SCALE)  // if mouse has the same y coord as buttons
                {
                    if (x > 110 * SCALE && x < 129 * SCALE)  // play button
                    {
                        *continueStartscreen = false;
                        sound->playConfirmationSound();
                    }
                    if (x > 159 * SCALE && x < 180 * SCALE)  // credits button
                    {
                        sound->RickRoll();
                        gui->play_credits(map, event);
                        sound->playLobbyMusic();
                    }
                    if (x > 59 * SCALE && x < 80 * SCALE)  // close button
                    {
                        sound->playConfirmationSound();
                        *continueStartscreen = false;
                        *gameover = true;
                        *endgame = true;
                    }
                } else if(y > 0 && y < 115 * SCALE){  // y coords of banners
                if((x > 5 * SCALE && x < 20 * SCALE)) {  // qwerty button
                    *layout_qwerty = true;
                    sound->playConfirmationSound();
                } else if((x > 219 * SCALE && x < 234 * SCALE)) {  // azerty button
                    *layout_qwerty = false;
                    sound->playConfirmationSound();
                }
            }
            break;
            }
        }
    }
}

void startscreen(Map* map, GUI* gui, Sound* sound, bool* gameover, SDL_Renderer *renderer, bool* endgame, bool* layout_qwerty, int* frameCount, Uint32* startTime)
{
    bool continueStartscreen = 1;
    while(continueStartscreen)
    {
        int timeOnStart = SDL_GetTicks();
        reduce_FPS(timeOnStart); // rerducing FPS to 60
        (*frameCount)++;

        gui->draw_start_screen();
        gui->draw_buttons(*layout_qwerty);  // draws highlightings for buttons

        SDL_Event event;
        handle_startscreen_events(map, gui, sound, &event, gameover, &continueStartscreen, endgame, renderer, layout_qwerty);

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

// seting spawn coficient to increase difficulty of game
void update_spawn_coef(clock_t* timer)
{
    if(update_score(timer) > 500 && update_score(timer) < 1000)
    {
        spawn_coef = 4;
    } 
    else if(update_score(timer) > 1000 && update_score(timer) < 1500) 
    {
        spawn_coef = 3;
    } 
    else if(update_score(timer) > 1500 && update_score(timer) < 2000) 
    {
        spawn_coef = 2;
    } 
    else if(update_score(timer) > 2000) 
    {
        spawn_coef = 1;
    }
}

void main_loop(bool gameover, int* frameCount, Uint32* startTime, SDL_Renderer *renderer, bool* endgame, bool* logo_shown, bool* layout_qwerty)
{
    Map map(renderer);
    try {
        map.calculate_map();  // placing all the objects on the map
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return;
    }

    // creating all the objects
    Owl owl(map.getOwlX(), map.getOwlY(), renderer);
    Poop poop(map.getOwlX(), map.getOwlY(), renderer);
    GUI gui(renderer);
    Sound sound;

    clock_t timer;

    Hunterlist* hunterListHead = nullptr;
    hunterListHead->createHunters(map.getGrassY(), HUNTERS_AMOUNT_ON_START, hunterListHead, renderer);
    TimeStamp spawn_timestamp = Clock::now();

    // finding the highest score achieved
    read_highscore();
    int highscore = highscores[0];

    sound.playLobbyMusic();
    if(!*logo_shown) {
        gui.draw_logo();
        SDL_RenderPresent(renderer);
        *logo_shown = true;
        SDL_Delay(3000);
    }

    // playing the start screen
    startscreen(&map, &gui, &sound, &gameover, renderer, endgame, layout_qwerty, frameCount, startTime);
    SDL_Delay(1000);

    if (!gameover)
    {
        sound.playGameLoopMusic();
    }

    timer = clock();

    while (!gameover) {
        int timeOnStart = SDL_GetTicks();  // to control fps

        owl.handle_keyboard(*layout_qwerty); // no need for the event variable, direct keyboard state polling

        SDL_Event event; // handle window closing
        handle_events(&event, &sound, &gameover, endgame);

        update_game(&owl, hunterListHead, &poop, &gui, &sound, highscore, &map, renderer, &gameover, &timer);
        draw(&owl, hunterListHead, &poop, &gui, highscore, &map, renderer, gameover, &sound);

        // handling fps
        reduce_FPS(timeOnStart); // rerducing FPS to 60
        (*frameCount)++;
        fps = count_FPS(startTime, frameCount, fps);

        update_spawn_coef(&timer);
        
        // spawning hunters depending on score to increase difficulty
        if(update_score(&timer) % spawn_coef == 0 && (std::chrono::duration<double>(Clock::now()-spawn_timestamp).count() > HUNTER_SPAWN_DELAY) && update_score(&timer) != 0) {
            hunterListHead->addHunter(map.getGrassY(), hunterListHead, renderer);
            spawn_timestamp = Clock::now();
        }
    }
    hunterListHead->freeHunterList(hunterListHead);
    write_highscore(score);
    score = 0;
}

int main()
{
    SDL_Renderer *renderer;
    SDL_Window *window;

    
    init_sdl(&window, &renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    TTF_Init();
    
    srand(time(NULL));

    bool endgame = false;
    bool logo_shown = 0;  // used once to show the logo on the first launch
    bool layout_qwerty = true;

    while(!endgame) { 
        bool gameover = false; 
        main_loop(gameover, &frameCount, &startTime, renderer, &endgame, &logo_shown, &layout_qwerty);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
