#include <cstdlib>
#include <unistd.h>


#include "poop.hpp"
#include "sprite.hpp"
#include "map.hpp"


class Bullet {
    public:
    Bullet(SDL_Renderer *renderer) : renderer(renderer), sprite(renderer, "bullet.bmp", BULLET_WIDTH){}

    void draw() {
        SDL_Rect src = sprite.rect(0);
        SDL_Rect dest = {int(x), int(y), BULLET_WIDTH, BULLET_HEIGHT};
        SDL_RenderCopyEx(renderer, sprite.texture, &src, &dest, angle*180/PI, nullptr, SDL_FLIP_NONE);
    }

    // while bullet is waiting for its time to be shot its position is linked to the hunter
    void follow(double hunterX, double hunterY) {
        x = hunterX + HUNTER_WIDTH/2 - BULLET_WIDTH/2;
        y = (hunterY + BULLET_HEIGHT);
    }

    // calculating an angle between the bullet and the owl
    double getDestAngle(double owlX, double owlY) {
        double l = (owlX + OWL_WIDTH/2) - (x + BULLET_WIDTH/2);
        if (l == 0) return 0;  
        double h = y - (owlY + OWL_HEIGHT/2);
        return atan(l/h);
    }

    void move(double angle) {
        x += BULLET_SPEED * sin(angle);
        y -= BULLET_SPEED * cos(angle);
    }

    void shoot(double owlX, double owlY) {
        // we set a delay for a bullet to be shot only after a certaint amount of time and not just after its return to hunter's position
        if (std::chrono::duration<double>(Clock::now()-shot_timestamp).count() > shooting_delay) {
            reset_shooting_delay();
            shot = true;
        }
    }

    void handle_collision(double owlX, double owlY) {
        // if bullet goes out of the screen
        if (y <= 0) {
            shot = false;
            shot_timestamp = Clock::now();
        }

        // if bullet hits the owl
        if (!(x > owlX + OWL_WIDTH || owlX > x + BULLET_WIDTH || y > owlY + OWL_HEIGHT || owlY > y + BULLET_HEIGHT)) {
            killed = 1;
            shot = false;
            shot_timestamp = Clock::now();
        }
    }

    void update_state(double hunterX, double hunterY, double owlX, double owlY) {
        handle_collision(owlX, owlY);
        if (!shot) {
            angle = getDestAngle(owlX, owlY);
            shoot(owlX, owlY);
            follow(hunterX, hunterY);
        } else {
            move(angle);
        }
    }

    void reset_shooting_delay()
    {
        shooting_delay = (SHOOTING_DELAY_MIN + ((double)rand() / RAND_MAX) * SHOOTING_DELAY_MAX);
    }

    bool getKilled() {
        return killed;
    }

    void setKilled(bool b) {
        killed = b;
    }


    private:
    double x = SCREEN_WIDTH/2 - BULLET_WIDTH/2, y = SCREEN_HEIGHT - HUNTER_HEIGHT + BULLET_HEIGHT; // coordinates of the bullet

    bool shot = 0;
    bool killed = 0;
    double angle = 0;
    TimeStamp shot_timestamp = Clock::now();
    double shooting_delay = (SHOOTING_DELAY_MIN + ((double)rand() / RAND_MAX) * SHOOTING_DELAY_MAX);

    SDL_Renderer *renderer;   // draw here

    const Sprite sprite;      // hunter sprite
};

struct Hunter {
    public :
    Hunter(SDL_Renderer *renderer) : renderer(renderer), sprite(renderer, "hunter.bmp", HUNTER_WIDTH), bullet(renderer){}
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

    void moveHunter(Map* map) {
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

    private:    
    double x = LEAVES_WIDTH + rand() % (SCREEN_WIDTH - HUNTER_WIDTH - LEAVES_WIDTH), y = SCREEN_HEIGHT - HUNTER_HEIGHT; // coordinates of the character

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

void freeHunterList(Hunterlist* &head) {
    Hunterlist* current_hunter = head;
    while (current_hunter != nullptr) {
        Hunterlist* next = current_hunter->next;
        delete current_hunter;
        current_hunter = next;
    }
}


void removeHunter(Hunterlist* &head, Hunter& hunter) {
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

void checkHunterCollision(Owl* owl, Hunterlist* &head, Poop* poop) {
    Hunterlist* current_hunter = head;
    while (current_hunter != nullptr) {
        if((poop->getCoordY() >= current_hunter->hunter.getCoordY() - HUNTER_HEIGHT/3) &&
         ((poop->getCoordX() >= current_hunter->hunter.getCoordX() - HUNTER_WIDTH) && 
         (poop->getCoordX() <= current_hunter->hunter.getCoordX() + HUNTER_WIDTH))) {
            if (head != NULL){
                removeHunter(head, current_hunter->hunter);
            }
            poop->reset(owl);
        }
        current_hunter = current_hunter->next;
    }
}

void moveHunters(Hunterlist* &head, Map* map) {
    Hunterlist* current_hunter = head;
    while (current_hunter != nullptr) {
        current_hunter->hunter.moveHunter(map);
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
