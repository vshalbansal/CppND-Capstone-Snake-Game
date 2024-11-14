#include "food.h"

Food::Food(int x, int y, int randomiser)
    : size(1){
    point.x = x;
    point.y = y;
    // 5/100 chance for big food
    if(randomiser<=5) {
        type = FoodType::BIG;
        color = {0,255,0,255};//Green
        size = 2;
    }
    // 5/100 chance for magic food
    else if(randomiser<=10) {
        type = FoodType::MAGIC;
        color = {0,0,255,255};//Blue
    }
    // 90/100 chance for normal food
    else {
        type = FoodType::NORMAL;
        color = {255,255,0,255};// Yellow
    }
}

bool Food::IsOccupied(int x, int y) {
    if( (x == point.x || x == point.x+size-1) && (y==point.y || y == point.y+size-1))
        return true;
    else
        return false;
}