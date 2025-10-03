#ifndef GAME_MODULE
#define GAME_MODULE

#include <ncurses.h>
#include "vector.h"
#include "player.h"
#include "bomb.h"

template<typename TYPE>
int SizeOfArray(TYPE **Array)
{
    int ArraySize = 0;
    while((Array[ArraySize]))
    {
        ArraySize++;
    }
    return ArraySize;
}

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
    Player[PlayerNum].setType(PlayerType);
    return (player&) Player[PlayerNum];
}

bomb &game::GetBomb(int BombNum)
{
    Bomb[BombNum].setType(BombType);
    return (bomb&) Bomb[BombNum];
}

player *game::GetPlayerArray()
{
    int ArraySize = SizeOfArray<player>(&Player);
    for(int i = 0; i < ArraySize; i++)
    {
        Player[i].setType(PlayerType);
    }
    return Player;
}

bomb *game::GetBombArray()
{
    int ArraySize = SizeOfArray<bomb>(&Bomb);
    for(int i = 0; i < ArraySize; i++)
    {
        Bomb[i].setType(BombType);
    }
    return Bomb;
}

#endif