#include <iostream>
#include <SDL2/SDL.h>
#include <chrono>
#include <algorithm>
#include "settings.hpp"


using namespace std;

#ifndef SPRITE_HPP
#define SPRITE_HPP

class Sprite
{
    public :  // we make it accessible to other classes
    Sprite(SDL_Renderer *renderer, const std::string filename, const int width) : width(width) {
        SDL_Surface *surface = SDL_LoadBMP((std::string(SPRITES_DIR) + filename).c_str());
        if (!surface) {
            std::cerr << "Error in SDL_LoadBMP: " << SDL_GetError() << std::endl;
            return;
        }
        if (!(surface->w%width) && surface->w/width) { // image width must be a multiple of sprite width
            height  = surface->h;
            nframes = surface->w/width;
            texture = SDL_CreateTextureFromSurface(renderer, surface);
        } else
            std::cerr << "Incorrect sprite size in " << filename << std::endl;
        SDL_FreeSurface(surface);
    }

    SDL_Rect rect(const int idx) const { // choose the sprite number idx from the texture
        return { idx*width, 0, width, height };
    }

    ~Sprite() { // texture destructor
        if (texture) SDL_DestroyTexture(texture);
    }

    SDL_Texture *texture; // the image is to be stored here
    int width   = 0; // single sprite width (texture width = width * nframes)
    int height  = 0; // sprite height
    int nframes = 0; // number of frames in the animation sequence
};

using Clock = std::chrono::high_resolution_clock;
using TimeStamp = std::chrono::time_point<Clock>;

class Animation : public Sprite {
    public :  // we make it accessible to other classes
    Animation(SDL_Renderer *renderer, const std::string filename, const int width, const double duration, const bool repeat) :
        Sprite(renderer, filename, width), duration(duration), repeat(repeat) {}

    bool animation_ended(const TimeStamp timestamp) const { // is the animation sequence still playing?
        double elapsed = std::chrono::duration<double>(Clock::now() - timestamp).count(); // seconds from timestamp to now
        return !repeat && elapsed >= duration;
    }

    int frame(const TimeStamp timestamp) const { // compute the frame number at current time for the the animation started at timestamp
        double elapsed = std::chrono::duration<double>(Clock::now() - timestamp).count(); // seconds from timestamp to now
        int idx = static_cast<int>(nframes*elapsed/duration);
        return repeat ? idx % nframes : std::min(idx, nframes-1);
    }

    SDL_Rect rect(const TimeStamp timestamp) const { // choose the right frame from the texture
        return { frame(timestamp)*width, 0, width, height };
    }

    const double duration = 1; // duration of the animation sequence in seconds
    const bool repeat = false; // should we repeat the animation?
};

#endif
