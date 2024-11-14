#ifndef FOOD_H
#define FOOD_H

#include "SDL.h"

enum class FoodType {NORMAL, MAGIC, BIG};

class Food {
    public:
        Food(int x, int y, int randomiser);
        bool IsOccupied(int x, int y);
        SDL_Point point;
        int size;
        SDL_Color color;
        FoodType type;
};

#endif