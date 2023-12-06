#include <iostream>
#include <fstream> 
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h> // sudo apt-get install libsdl2-mixer-dev libogg-dev libvorbis-dev   !!! ЗНИЩИТИ ПІСЛЯ ВИКОРИСТАННЯ !!!
#include "owl.hpp"
#include "hunter.hpp"
#include "settings.hpp"
#include "poop.hpp"
#include "gui.hpp"
#include "map.hpp"

using namespace std;

int number = 3;
int frameCount = 0; // Global frame count
Uint32 startTime = 0; // Global start time
int fps;
int score = 0;


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

void reduce_FPS(int timeOnStart)
{
    int delta = SDL_GetTicks() - timeOnStart;
    int desiredDelta = 1000 / FPS_LIM;

    if (delta < desiredDelta)
    {
        SDL_Delay(desiredDelta - delta);    // FPS reducing
    }
}

void count_FPS(Uint32* startTime, int* frameCount)
{
    Uint32 currentTime = SDL_GetTicks();
    Uint32 deltaTime = currentTime - *startTime;

    if (deltaTime >= 1000) 
    {
        fps = static_cast<float>(*frameCount) / (static_cast<float>(deltaTime) / 1000.0f);
        *frameCount = 0;
        *startTime = currentTime;
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

int read_highscore()
{
    fstream myfile("highscore.txt", std::ios_base::in);
    int num;
    if (!myfile.is_open()) {
        num = 0; // Set highscore to 0 if file doesn't exist
    } else {
        myfile >> num; // Read highscore from the file
    }
    myfile.close(); // Close the file

    return num;
}

void write_highscore() 
{
    if (score > read_highscore()){
        ofstream outfile("highscore.txt", std::ios_base::trunc);
        outfile << score << "\n";
        outfile.close();
    }
}

void hunter_sound(){
    Mix_Chunk* hunter = Mix_LoadWAV("resources/hunter.ogg");
    Mix_PlayChannel(-1, hunter, 0);
}

void draw(Owl* owl, Hunterlist * &list, Poop* poop, GUI* gui, int highscore, Map* map, SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);
    map->draw_background();
    if(list->checkHunterCollision(owl, list, poop, renderer)){
        score += 20;
        hunter_sound();
    }
    gui->draw_score(score, highscore);
    poop->draw();
    list->moveHunters(map, list);
    list->drawHunters(list);
    map->draw_surrounding();
    owl->draw();
    gui->draw_gui(owl->getLives(), fps);
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
            } else {
                *gameover = true;
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
        return pooped;
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
                    }
                    if (y > gui->getCreditsY() && y < gui->getCreditsY() + BUTTON_HEIGHT)
                    {
                        // to complete
                    }
                    if (y > gui->getExitY() && y < gui->getExitY() + BUTTON_HEIGHT)
                    {
                        *continueStartscreen = false;
                        *gameover = true;
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
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        fprintf(stderr, "Mix_OpenAudio failed: %s\n", Mix_GetError());
        SDL_Quit();
    }

    Mix_Music* game_loop = Mix_LoadMUS((string(AUDIO_DIR) + string("game_loop.ogg")).c_str());
    Mix_Music* start_screen = Mix_LoadMUS((string(AUDIO_DIR) + string("start_screen.ogg")).c_str());
    Mix_Music* confirm = Mix_LoadMUS((string(AUDIO_DIR) + string("confirm.ogg")).c_str());
    Mix_Music* death = Mix_LoadMUS((string(AUDIO_DIR) + string("death.ogg")).c_str());
    Mix_Chunk* hit = Mix_LoadWAV((string(AUDIO_DIR) + string("hit.ogg")).c_str());
    Mix_Chunk* pooped = Mix_LoadWAV((string(AUDIO_DIR) + string("poop.ogg")).c_str());


    Map map(renderer);

    try
    {
        map.calculate_map();
    }
    catch(const std::exception& e)
    {
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
    int highscore = read_highscore();
    int state;

    Mix_PlayMusic(start_screen, -1);
    startscreen(&map, &gui, &gameover, renderer);
    Mix_PlayMusic(confirm, 1);
    SDL_Delay(1000);
    Mix_FreeMusic(start_screen);
    Mix_FreeMusic(confirm);

    Mix_PlayMusic(game_loop, -1);

    
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
        reduce_FPS(timeOnStart);
        (*frameCount)++;
        count_FPS(startTime, frameCount);

        if(update_score(&timer) % 5 == 0 && (std::chrono::duration<double>(Clock::now()-spawn_timestamp).count() > HUNTER_SPAWN_DELAY) && update_score(&timer) != 0) {
            hunterListHead->addHunter(map.getGrassY(), hunterListHead, renderer);
            spawn_timestamp = Clock::now();
        }
    }
    hunterListHead->freeHunterList(hunterListHead);
    Mix_FreeMusic(game_loop);
    Mix_PlayMusic(death, 1);
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

    write_highscore();

    SDL_Delay(2500);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    Mix_CloseAudio();
    return 0;
}
