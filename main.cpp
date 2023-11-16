#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "owl.hpp"
#include "hunter.hpp"
#include "settings.hpp"
#include "poop.hpp"
#include "gui.hpp"

using namespace std;


SDL_Texture* background = NULL;  // background texture is global variable to be valid from any place of a code

int frameCount = 0; // Global frame count
Uint32 startTime = 0; // Global start time
int fps;


SDL_Texture *load_image(const char path[], SDL_Renderer *renderer)
{
    SDL_Surface *tmp = NULL;
    SDL_Texture *texture = NULL;
    tmp = SDL_LoadBMP(path);
    if(NULL == tmp)
    {
        fprintf(stderr, "Error while charging an image BMP: %s", SDL_GetError());
        return NULL;
    }
    SDL_SetColorKey(tmp, SDL_TRUE, SDL_MapRGB(tmp->format, 255, 0, 255));
    texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    if(NULL == texture)
    {
        fprintf(stderr, "Error while creating a texture: %s", SDL_GetError());
        return NULL;
    }
    return texture;
}

void init_background(SDL_Renderer *renderer)
{
    background = load_image( "resources/background.bmp", renderer);
}

void apply_background(SDL_Renderer *renderer)
{
    if (background != NULL)
    {
      SDL_RenderCopy(renderer, background, NULL, NULL);
    }
}

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
            if (event->key.keysym.sym == SDLK_ESCAPE) // ESC to exit
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
        SDL_Delay(desiredDelta - delta); // FPS reducing
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

void draw(Owl* owl, Node * list, Poop* poop, GUI* gui, SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);
    apply_background(renderer);
    checkHunterCollision(owl, list, poop);
    drawHunters(list);
    poop->draw();
    owl->draw();
    gui->draw_lives();
    gui->apply_lives_text(renderer, owl->getLives());
    gui->apply_fps(renderer, fps);
    SDL_RenderPresent(renderer);
}


void update_game(Owl* owl, Node* list, Poop* poop, GUI* gui, SDL_Renderer *renderer, bool* gameover) {
    owl->update_state();
    poop->update_state(owl);
    Node* current = list;
    while (current != nullptr) {
        Bullet* bullet = current->hunter.getBulletAdr();
        updateHuntersWithBullets(current, owl); 
        if (bullet->getKilled()) { 
            owl->shot();
            if (owl->getLives() > 0) { 
                bullet->setKilled(0);
            } else {
                *gameover = true;
            }
            poop->reset(owl); 
            poop->update_state(owl);
            draw(owl, list, poop, gui, renderer); 
            break;
        }
        current = current->next; // Move to next hunter
    }
}


void main_loop(bool gameover, int* frameCount, Uint32* startTime, SDL_Renderer *renderer, SDL_Texture* background)
{
    Owl owl(renderer);
    Poop poop(renderer);
    GUI gui(renderer);

    Node* hunterListHead = nullptr;
    Hunter hunter1(renderer);
    insertHunter(hunterListHead, hunter1);
    Hunter hunter2(renderer);
    insertHunter(hunterListHead, hunter2);
    Hunter hunter3(renderer);
    insertHunter(hunterListHead, hunter3);

    while (!gameover) {
        int timeOnStart = SDL_GetTicks();

        owl.handle_keyboard(); // no need for the event variable, direct keyboard state polling
        SDL_Event event; // handle window closing
        handle_events(&event, &gameover);
        update_game(&owl, hunterListHead, &poop, &gui, renderer, &gameover);
        draw(&owl, hunterListHead, &poop, &gui, renderer);
        reduce_FPS(timeOnStart);
        (*frameCount)++;
        count_FPS(startTime, frameCount);
    }
    freeHunterList(hunterListHead);
}

int init_sdl(SDL_Window **window, SDL_Renderer **renderer, int width, int height)
{
    if(0 != SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO))
    {
        fprintf(stderr, "Error while initialising SDL: %s", SDL_GetError());
        return -1;
    }
    if(0 != SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, window, renderer))
    {
        fprintf(stderr, "Error while creating an image and rendering it: %s", SDL_GetError());
        return -1;
    }SDL_DestroyTexture(background);
    return 0;
}



int main()
{
    SDL_Renderer *renderer;
    SDL_Window *window;

    init_sdl(&window, &renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    init_background(renderer);
    TTF_Init();
    
    bool gameover = false;

    srand(time(NULL));

    main_loop(gameover, &frameCount, &startTime, renderer, background);

    SDL_Delay(1000);
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();    
    return 0;
}