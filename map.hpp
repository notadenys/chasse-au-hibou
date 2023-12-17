#ifndef MAP_HPP
#define MAP_HPP

#include "game_support.hpp"
#include "settings.hpp"
#include "sprite.hpp"


class Map
{
    public:
        Map(SDL_Renderer *renderer) : renderer(renderer), 
        background(renderer, "background.bmp", BACKGROUND_WIDTH/SCALE),
        grass(renderer, "grass.bmp", GRASS_WIDTH/SCALE), 
        tree(renderer, "tree.bmp", TREE_WIDTH/SCALE){}

        void draw_background() {
            SDL_Rect bgR = {0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT};
            SDL_RenderCopy(renderer, background.texture, NULL, &bgR);
        }

        void draw_surrounding()
        {
            for(int i = 0; i < nbTrees; i++)
            {
                SDL_Rect treeR = {treesX[i], treeY, TREE_WIDTH, TREE_HEIGHT};
                SDL_RenderCopy(renderer, tree.texture, NULL, &treeR);
            }

            SDL_Rect grassR = {grassX, grassY, GRASS_WIDTH, GRASS_HEIGHT};
            SDL_RenderCopy(renderer, grass.texture, NULL, &grassR);
        }

        bool left_collision(double x, double w)
        {
            for(int i = 0; i < nbTrees; i++)
            {
                if (treesX[i] + TREE_WIDTH/1.5 > x && treesX[i] + TREE_WIDTH/1.5 < x + w)
                {
                    return true;
                }
            }
            return false;
        }

        bool right_collision(double x, double w)
        {
            for(int i = 0; i < nbTrees; i++)
            {
                if (treesX[i] + TREE_WIDTH/2.1 > x && treesX[i] + TREE_WIDTH/2.1 < x + w)
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
                        case '1':  // grass
                            if (grassY < 0)
                            {
                                grassY = (SCREEN_HEIGHT / nbLig) * i;
                            }
                            else
                            {
                                throw std::invalid_argument("Redefinition of grass");
                            }
                            break;

                        case '2':  // owl
                            // coordinates are set by the first occurence of 2
                            if (owlX < 0)
                            {
                                owlX = (SCREEN_WIDTH / nbCol) * j;
                                owlY = (SCREEN_HEIGHT / nbLig) * i;
                            }
                            else
                            {
                                throw std::invalid_argument("Redefinition of owl");
                            }
                            break;

                        case '3':  //trees
                            treesX[nbTreesBuilt] = SCREEN_WIDTH / nbCol * j - TREE_WIDTH/2;
                            nbTreesBuilt++;

                        default:
                            break;
                        }
                }
            }

            if (grassY < 0)
            {
                throw std::invalid_argument("Ground is not defined");
            }
            if (owlX < 0)
            {
                throw std::invalid_argument("Owl is not defined");
            }

            desallocate_tab_2D(tab, nbLig);
        }

        int getGrassY()
        {
            return grassY;
        }

        int getOwlX()
        {
            return owlX;
        }

        int getOwlY()
        {
            return owlY;
        }

        ~Map()
        {
            delete treesX;
        }

    private:
        // coordinates of the objects on the screes (predefined -1)
        int grassX = 0, grassY = -1;
        int owlX = -1, owlY = -1;
        int nbTrees = count_trees();
        int* treesX = new int(nbTrees);
        int treeY = 0;

        SDL_Renderer *renderer; // draw here

        const Sprite background;
        const Sprite grass;
        const Sprite tree;
};
#endif
