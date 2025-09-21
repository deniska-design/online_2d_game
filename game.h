#ifndef GAME_MODULE
#define GAME_MODULE

#include <ncurses.h>
#include "vector.h"
#include "player.h"
#include "bomb.h"

class game
{
    player *Player;
    bomb *Bomb;
public:
    game();
    game(int MaxPlayerCount, int MaxBombCount);
    player &GetPlayer(int PlayerNum);
    bomb &GetBomb(int BombNum);
    player *GetPlayerArray();
    bomb *GetBombArray();
};

game::game()
{
    Player = new player[5];
    Bomb = new bomb[1];
}

game::game(int MaxPlayerCount, int MaxBombCount)
{
    Player = new player[MaxPlayerCount];
    Bomb = new bomb[MaxBombCount];
}

player &game::GetPlayer(int PlayerNum)
{
    return (player&) Player[PlayerNum];
}

bomb &game::GetBomb(int BombNum)
{
    return (bomb&) Bomb[BombNum];
}

player *game::GetPlayerArray()
{
    return Player;
}

bomb *game::GetBombArray()
{
    return Bomb;
}

#endif