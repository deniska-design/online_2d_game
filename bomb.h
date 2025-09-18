#ifndef BOMB_MODULE
#define BOMB_MODULE

#include <ncurses.h>
#include "vector.h"

class bomb
{
    Vector position;
    int BombHigh, BombWidth;
    bool active;
public:
    bomb();
    bomb(bool state, Vector position, int NewBombHigh, int NewBombWidth);
};

bomb::bomb()
{
    position = Vector{0, 0};
    active = false;
    BombHigh = 2;
    BombWidth = 2;
}

bomb::bomb(bool state, Vector position, int NewBombHigh, int NewBombWidth)
{
    active = state;
    position = position;
    BombHigh = NewBombHigh;
    BombWidth = NewBombWidth;
}

#endif