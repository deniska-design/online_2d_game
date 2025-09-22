#ifndef BOMB_MODULE
#define BOMB_MODULE

#include "object.h"

enum 
{
    exploded	=	disactiv,      
};

class bomb : public object
{
public:
    bomb();
    bomb(bool state, Vector position, int BombHigh, int BombWidth);
};

bomb::bomb()
{
    position = Vector{0, 0};
    High = 2;
    Width = 2;
    statue = disactiv;
}

bomb::bomb(bool state, Vector NewPosition, int BombHigh, int BombWidth)
{
    position = NewPosition;
    High = BombHigh;
    Width = BombWidth;
    statue = state;
}

#endif