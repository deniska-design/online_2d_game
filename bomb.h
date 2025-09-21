#ifndef BOMB_MODULE
#define BOMB_MODULE

#include <ncurses.h>
#include "vector.h"

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
    ActiveOrNot = disactiv;
}

bomb::bomb(bool state, Vector NewPosition, int BombHigh, int BombWidth)
{
    position = NewPosition;
    High = BombHigh;
    Width = BombWidth;
    ActiveOrNot = state;
}

#endif