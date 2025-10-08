#ifndef BOMB_MODULE
#define BOMB_MODULE

#include "object.h"

enum 
{
    exploded	=	disactiv,     

    DefaultBombHigh     = 2,
    DefaultBombWidth    = 2, 
};

class bomb : public object
{
public:
    bomb();
    bomb(bool state, Vector position, int BombHigh, int BombWidth);
};

bomb::bomb()
{
    position = Vector{DefaultPositionX, DefaultPositionY};
    High = DefaultBombHigh;
    Width = DefaultBombWidth;
    statue = disactiv;
    ObjectType = BombType;
}

bomb::bomb(bool state, Vector NewPosition, int BombHigh, int BombWidth)
{
    position = NewPosition;
    High = BombHigh;
    Width = BombWidth;
    statue = state;
    ObjectType = BombType;
}

#endif