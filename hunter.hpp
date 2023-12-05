#ifndef HUNTER_HPP
#define HUNTER_HPP

#include <cstdlib>
#include <unistd.h>

#include "poop.hpp"
#include "bullet.hpp"
#include "sprite.hpp"
#include "map.hpp"

struct Hunterlist;


struct Hunter {
    public :
    Hunter(int y, SDL_Renderer *renderer) : y(y-HUNTER_HEIGHT/2), renderer(renderer), sprite(renderer, "hunter.bmp", HUNTER_WIDTH/SCALE), bullet(renderer){}
        void draw() { // drawind hunter, if alive
        SDL_Rect src = sprite.rect(0);
        SDL_Rect dest = {int(x), int(y), HUNTER_WIDTH, HUNTER_HEIGHT};
        SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, 0, nullptr, SDL_FLIP_NONE);
    }

    double getCoordX() { // getting coordinate X
        return x;
    }

    double getCoordY() { // getting coordinate Y
        return y;
    }

    Bullet* getBulletAdr() {
        return &bullet;
    }

    void moveHunter(Map* map, Hunterlist* list) {
        int change_dir = rand() % 100;
        if((change_dir == 0 && direction == 0) || (map->right_collision(x, HUNTER_WIDTH) && direction == 0)) {
            direction = 1;
        }
        else if((change_dir == 0 && direction == 1) || (map->left_collision(x, HUNTER_WIDTH) && direction == 1)) {
            direction = 0;
        }
        if(direction == 0) {
            x += HUNTER_SPEED;
        }
        if(direction == 1) {
            x -= HUNTER_SPEED;
        }
    }

    void setY(int h)
    {
        y = h;
    }

    private:    
    double x = TREE_WIDTH + rand() % (SCREEN_WIDTH - HUNTER_WIDTH - TREE_WIDTH*2), y; // coordinates of the character

    int direction = rand() % 2;

    SDL_Renderer *renderer; // draw here

    const Sprite sprite; // hunter sprite

    Bullet bullet; // bullet, linked to hunter
};



struct Hunterlist { // linked list, to store several hunters
    public :
    Hunter hunter;
    Hunterlist* next;

    Hunterlist(Hunter& hunter) : hunter(hunter) {}
    
};

void insertHunter(Hunterlist* &head, Hunter& hunter) {
    Hunterlist* newHunterlist = new Hunterlist(hunter);

    newHunterlist->next = head;
    head = newHunterlist;
}

void addHunter(int y, Hunterlist* &head, SDL_Renderer *renderer) {
    Hunter* hunter = new Hunter(y, renderer);
    insertHunter(head, *hunter);
}

void createHunters(int y, int numHunters, Hunterlist* &head, SDL_Renderer* renderer) {
    for (int i = 0; i < numHunters; i++) {
        Hunter* hunter = new Hunter(y, renderer);
        insertHunter(head, *hunter);
    }
}

void freeHunterList(Hunterlist* &head) {
    Hunterlist* current_hunter = head;
    while (current_hunter != nullptr) {
        Hunterlist* next = current_hunter->next;
        delete current_hunter;
        current_hunter = next;
    }
}

void removeHunter(Hunterlist* &head, Hunter& hunter, SDL_Renderer *renderer) {
    Hunterlist* current_hunter = head;
    Hunterlist* previous_hunter = nullptr;
    while (current_hunter != nullptr) {
        if (current_hunter->hunter.getCoordX() == hunter.getCoordX() && current_hunter->hunter.getCoordY() == hunter.getCoordY()) {
            if (current_hunter != nullptr) {
                if(current_hunter == head) {
                    if(current_hunter->next != nullptr) {
                        head = current_hunter->next;
                        delete current_hunter;
                    }
                    else {
                        head = current_hunter->next;
                        delete current_hunter;
                    }
                }
                else if(current_hunter->next == nullptr) {
                    previous_hunter->next = nullptr;
                    delete current_hunter;
                }
                else if(previous_hunter != nullptr) {
                    previous_hunter->next = current_hunter->next;
                    delete current_hunter;
                }
            }
            break;
        }
        previous_hunter = current_hunter;
        current_hunter = current_hunter->next;
        previous_hunter->next = current_hunter;
    }
}

void drawHunters(Hunterlist* &head) {
    Hunterlist* current_hunter = head;
    while (current_hunter != nullptr) {
        current_hunter->hunter.getBulletAdr()->draw();
        current_hunter->hunter.draw();
        current_hunter = current_hunter->next;
    }
}

bool checkHunterCollision(Owl* owl, Hunterlist* &head, Poop* poop, SDL_Renderer *renderer) {
    bool killed = false;
    Hunterlist* current_hunter = head;
    while (current_hunter != nullptr) {
        if((poop->getCoordY() >= current_hunter->hunter.getCoordY() - HUNTER_HEIGHT/3) &&
         ((poop->getCoordX() >= current_hunter->hunter.getCoordX() - HUNTER_WIDTH) && 
         (poop->getCoordX() <= current_hunter->hunter.getCoordX() + HUNTER_WIDTH))) {
            if (head != NULL){
                removeHunter(head, current_hunter->hunter, renderer);
                killed = true;
            }
            poop->reset(owl);
        }
        current_hunter = current_hunter->next;
    }
    return killed;
}

void moveHunters(Map* map, Hunterlist* &head) {
    Hunterlist* current_hunter = head;
    while (current_hunter != nullptr) {
        current_hunter->hunter.moveHunter(map, head);
        current_hunter = current_hunter->next;
    }
}

void updateHunterWithBullet(Hunterlist* &head, Owl* owl) {
    Hunterlist* current_hunter = head;
    Bullet* bullet = current_hunter->hunter.getBulletAdr();

    if (bullet != nullptr) 
    {
        bullet->update_state(current_hunter->hunter.getCoordX(), current_hunter->hunter.getCoordY(), owl->getCoordX(), owl->getCoordY());
    }
}

#endif // HUNTER_HPP
