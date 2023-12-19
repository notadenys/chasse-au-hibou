#ifndef HUNTER_HPP
#define HUNTER_HPP

#include <cstdlib>
#include <unistd.h>

#include "poop.hpp"
#include "bullet.hpp"
#include "sprite.hpp"
#include "map.hpp"

class Hunterlist; // костиль

class Hunter {
    public :
    Hunter(int y, SDL_Renderer *renderer) : y(y-HUNTER_HEIGHT/2.5), 
                                            renderer(renderer), 
                                            hunter_sprite(renderer, "hunter.bmp", HUNTER_WIDTH/SCALE), 
                                            rifle_sprite(renderer, "rifle.bmp", RIFLE_WIDTH/SCALE), 
                                            bullet(renderer){}

    // drawind hunter, if alive
    void draw(Owl* owl) 
    {
        update_graphics(owl);
        SDL_Rect h_src = hunter_sprite.rect(0);
        SDL_Rect h_dest = {int(x), int(y), HUNTER_WIDTH, HUNTER_HEIGHT};
        SDL_RenderCopyEx(renderer, hunter_sprite.texture, &h_src, &h_dest, 0, nullptr, (looking_left) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        SDL_Rect r_src = rifle_sprite.rect(0);
        SDL_Rect r_dest = {int(x + HUNTER_WIDTH/2 - RIFLE_WIDTH/2) , int(y + HUNTER_HEIGHT/2 - RIFLE_HEIGHT/2), RIFLE_WIDTH, RIFLE_HEIGHT};
        SDL_RenderCopyEx(renderer, rifle_sprite.texture, &r_src, &r_dest, (looking_left) ? angle*180/PI + 30 : angle*180/PI - 30, nullptr, (looking_left) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }

    void update_graphics(Owl* owl)
    {
        looking_left = x + HUNTER_WIDTH/2 > owl->getCoordX() + OWL_WIDTH/2;

        angle = getDestAngle(owl);
    }

    // calculating an angle between the hunter and the owl
    double getDestAngle(Owl* owl) {
        double l = (owl->getCoordX() + OWL_WIDTH/2) - (x + HUNTER_WIDTH/2);
        if (l == 0) return 0;  
        double h = y + HUNTER_HEIGHT/2 - (owl->getCoordY() + OWL_HEIGHT/2);
        return atan(l/h);
    }

    double getCoordX() {
        return x;
    }

    double getCoordY() {
        return y;
    }

    double getAngle()
    {
        return angle;
    }

    Bullet* getBulletAdr() {
        return &bullet;
    }

    
    void moveHunter(Map* map, Hunterlist* list) {
        int change_dir = rand() % 100;  // 1% probability to change direction
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

    private:    
    double x = TREE_WIDTH/8 + rand() % (SCREEN_WIDTH - HUNTER_WIDTH - TREE_WIDTH/8), y; // coordinates of the character

    int direction = rand() % 2;
    bool looking_left = true;

    SDL_Renderer *renderer; // draw here

    const Sprite hunter_sprite;
    const Sprite rifle_sprite; 

    double angle = 0;

    Bullet bullet; // bullet, linked to hunter
};

class Hunterlist { // linked list, to store several hunters
    public :
    Hunter hunter;
    Hunterlist* next;

    Hunterlist(Hunter& hunter) : hunter(hunter) {}

    // adding one existing hunter in existing list
    void insertHunter(Hunterlist* &head, Hunter& hunter) {  
        Hunterlist* newHunterlist = new Hunterlist(hunter);
        newHunterlist->next = head;
        head = newHunterlist;
    }
    
    // adding new hunter in existing list
    void addHunter(int y, Hunterlist* &head, SDL_Renderer *renderer) { 
        Hunter* hunter = new Hunter(y, renderer);
        head->insertHunter(head, *hunter);

    }

    // adding N hunters in existing list
    void createHunters(int y, int numHunters, Hunterlist* &head, SDL_Renderer* renderer) { 
        for (int i = 0; i < numHunters; i++) {
            Hunter* hunter = new Hunter(y, renderer);
            head->insertHunter(head, *hunter);
        }
    }

    // delete list (when game ends)
    void freeHunterList(Hunterlist* &head) { 
        Hunterlist* current_hunter = head;
        while (current_hunter != nullptr) {
            Hunterlist* next = current_hunter->next;
            delete current_hunter;
            current_hunter = next;
        }
    }

    // delete concrete hunter (when poop hits hunter)
    void removeHunter(Hunterlist* &head, Hunter& hunter, SDL_Renderer *renderer) { 
        Hunterlist* current_hunter = head;
        Hunterlist* previous_hunter = nullptr;
        while (current_hunter != nullptr) {
            if (current_hunter->hunter.getCoordX() == hunter.getCoordX() && current_hunter->hunter.getCoordY() == hunter.getCoordY()) { 
                // check if current hunter is hitted one, else checking next
                if (current_hunter != nullptr) {
                    if(current_hunter == head) {
                        if(current_hunter->next != nullptr) {
                            // case if hunter is the first and only one
                            head = current_hunter->next;
                            delete current_hunter;
                        }
                        else {
                            // case if hunter is the first and there is next hunter
                            head = current_hunter->next;
                            delete current_hunter;
                        }
                    }
                    else if(current_hunter->next == nullptr) {
                        // case if hunter is the last one
                        previous_hunter->next = nullptr;
                        delete current_hunter;
                    }
                    else if(previous_hunter != nullptr) {
                        // case if hunter is in middle of list
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

    // drawing hunters from list
    void drawHunters(Hunterlist* &head, Owl* owl) {
        Hunterlist* current_hunter = head;
        while (current_hunter != nullptr) {
            current_hunter->hunter.draw(owl);
            current_hunter = current_hunter->next;
        }
    }

    // drawing hunters from list
    void drawBullets(Hunterlist* &head) {
        Hunterlist* current_hunter = head;
        while (current_hunter != nullptr) {
            current_hunter->hunter.getBulletAdr()->draw();
            current_hunter = current_hunter->next;
        }
    }

    // checking collision of hunter and poop, removing if true
    bool checkHunterCollision(Owl* owl, Hunterlist* &head, Poop* poop, SDL_Renderer *renderer) {
        bool killed = false;
        Hunterlist* current_hunter = head;
        while (current_hunter != nullptr) {
            if((poop->getCoordY() >= current_hunter->hunter.getCoordY() - HUNTER_HEIGHT/3) &&
            ((poop->getCoordX() >= current_hunter->hunter.getCoordX() - HUNTER_WIDTH) && 
            (poop->getCoordX() <= current_hunter->hunter.getCoordX() + HUNTER_WIDTH))) {
                if (head != NULL){
                    head->removeHunter(head, current_hunter->hunter, renderer);
                    killed = true;
                }
                poop->reset(owl);
            }
            current_hunter = current_hunter->next;
        }
        return killed;
    }

    // iteration to move each hunter of list
    void moveHunters(Map* map, Hunterlist* &head) {
        Hunterlist* current_hunter = head;
        while (current_hunter != nullptr) {
            current_hunter->hunter.moveHunter(map, head);
            current_hunter = current_hunter->next;
        }
    }

    // bullet follows hunter
    void updateHunterWithBullet(Hunterlist* &head, Owl* owl) {
        Hunterlist* current_hunter = head;
        Bullet* bullet = current_hunter->hunter.getBulletAdr();
        if (bullet != nullptr) 
        {
            bullet->update_state(current_hunter->hunter.getCoordX(), current_hunter->hunter.getCoordY(), current_hunter->hunter.getAngle(), owl);
        }
    }
};
#endif // HUNTER_HPP
