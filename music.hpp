#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "settings.hpp"

using namespace std;

Mix_Music* game_loop;
Mix_Music* start_screen;
Mix_Music* death;
Mix_Music* rickroll;
Mix_Chunk* confirm;
Mix_Chunk* hit;
Mix_Chunk* pooped;
Mix_Chunk* hunter;

// loading files and initializing of SDL_mixer
void loadMusic(){
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        fprintf(stderr, "Mix_OpenAudio failed: %s\n", Mix_GetError());
        SDL_Quit();
    }
    game_loop = Mix_LoadMUS((string(AUDIO_DIR) + string("game_loop.ogg")).c_str());
    start_screen = Mix_LoadMUS((string(AUDIO_DIR) + string("start_screen.ogg")).c_str());
    death = Mix_LoadMUS((string(AUDIO_DIR) + string("death.ogg")).c_str());
    rickroll = Mix_LoadMUS((string(AUDIO_DIR) + string("rickroll.ogg")).c_str());
    confirm = Mix_LoadWAV((string(AUDIO_DIR) + string("confirm.ogg")).c_str());
    hit = Mix_LoadWAV((string(AUDIO_DIR) + string("hit.ogg")).c_str());
    pooped = Mix_LoadWAV((string(AUDIO_DIR) + string("poop.ogg")).c_str());
    hunter = Mix_LoadWAV((string(AUDIO_DIR) + string("hunter.ogg")).c_str());
}

// playing music

void playLobbyMusic() {
    Mix_PlayMusic(start_screen, -1);
}

void playGameLoopMusic() {
    Mix_PlayMusic(game_loop, -1);
}

void playDeathMusic() {
    Mix_PlayMusic(death, 1);
}

void RickRoll() {
    Mix_PlayMusic(rickroll, -1);
}

// playing sounds

void playHunterSound(){
    Mix_PlayChannel(-1, hunter, 0);
}

void playPoopSound() {
    Mix_PlayChannel(-1, pooped, 0);
}

void playConfirmationSound() {
    Mix_PlayChannel(-1, confirm, 0);
}

void playHitSound() {
    Mix_PlayChannel(-1, hit, 0);
}
#endif