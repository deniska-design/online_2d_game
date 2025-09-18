#ifndef GAME_MODULE
#define GAME_MODULE

#include <ncurses.h>
#include "vector.h"
#include "player.h"
#include "bomb.h"

class game
{
    player *Player;
    bomb Bomb;
public:
    game();
    game(int MaxPlayerCount);
    player &operator[](int PlayerNum);
};

game::game()
{
    Player = new player[5];
}

game::game(int MaxPlayerCount)
{
    Player = new player[MaxPlayerCount];
}

player &game::operator[](int PlayerNum)
{
    return (player&) Player[PlayerNum];
}

#endif