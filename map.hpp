#include "game_support.hpp"
#include "settings.hpp"
#include "sprite.hpp"

#ifndef MAP_HPP
#define MAP_HPP

struct Map
{
    public:
        Map(SDL_Renderer *renderer) : renderer(renderer), 
        background(renderer, "background.bmp", SCREEN_WIDTH),
        ground(renderer, "ground.bmp", SCREEN_WIDTH), 
        tree(renderer, "tree.bmp", TREE_WIDTH),
        leaves(renderer, "leaves.bmp", LEAVES_WIDTH){}

        void draw_background()
        {
            SDL_RenderCopy(renderer, background.texture, NULL, NULL);
        }

        void draw()
        {
            SDL_Rect groundR = {groundX, groundY, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, ground.texture, NULL, &groundR);

            for(int i = 0; i < nbTrees; i++)
            {
                SDL_Rect treeR = {treesX[i], treeY, TREE_WIDTH, groundY};
                SDL_RenderCopy(renderer, tree.texture, NULL, &treeR);

                SDL_Rect leavesR = {treesX[i] - LEAVES_WIDTH/2 + TREE_WIDTH/2, 0, LEAVES_WIDTH, LEAVES_HEIGHT};
                SDL_RenderCopy(renderer, leaves.texture, NULL, &leavesR);
            }
        }

        bool left_collision(int x, int w)
        {
            for(int i = 0; i < nbTrees; i++)
            {
                if (treesX[i] + TREE_WIDTH > x && treesX[i] + TREE_WIDTH < x + w)
                {
                    return true;
                }
            }
            return false;
        }

        bool right_collision(int x, int w)
        {
            for(int i = 0; i < nbTrees; i++)
            {
                if (treesX[i] > x && treesX[i] < x + w)
                {
                    return true;
                }
            }
            return false;
        }
        
        /*counts the amount of the trees in text file*/
        int count_trees()
        {
            int nbLig = 0;
            int nbCol = 0;

            file_size("resources/terrain.txt", &nbLig, &nbCol);
            char** tab = lire_fichier("resources/terrain.txt");

            int counter = 0;

            for(int i = 0; i < nbLig; i++)
            {
                for(int j = 0; j < nbCol; j++)
                {
                    if(tab[i][j] == '3') counter++;
                }
            }
            return counter;
        }

        /*places all the objects on the map based on a file*/
        void calculate_map()
        {
            int nbLig = 0;
            int nbCol = 0;

            file_size("resources/terrain.txt", &nbLig, &nbCol);
            char** tab = lire_fichier("resources/terrain.txt");

            int nbTreesBuilt = 0;  // counter to iterate through trees array

            for(int i = 0; i < nbLig; i++)
            {
                for(int j = 0; j < nbCol; j++)
                {
                    switch (tab[i][j])
                    {
                    case '1':  // ground
                        groundY = (groundY < 0) ? (SCREEN_HEIGHT / nbLig) * i : groundY;  // coordinates are set by the first occurence of 1
                        break;
                    case '2':  // owl
                        // coordinates are set by the first occurence of 2
                        owlX = (owlX < 0) ? (SCREEN_WIDTH / nbCol) * j : owlX;
                        owlY = (owlY < 0) ? (SCREEN_HEIGHT / nbLig) * i : owlY;
                        break;
                    case '3':  //trees
                        treesX[nbTreesBuilt] = SCREEN_WIDTH / nbCol * j;
                        nbTreesBuilt++;
                    default:
                        break;
                    }
                }
            }
        }

    private:
        // coordinates of the objects on the screes (predefined -1)
        int groundX = -1, groundY = -1;
        int owlX = -1, owlY = -1;
        int nbTrees = count_trees();
        int* treesX = new int(nbTrees);
        int treeY = 50;

        SDL_Renderer *renderer; // draw here

        const Sprite background;
        const Sprite ground;
        const Sprite tree;
        const Sprite leaves;
};
#endif
