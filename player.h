#ifndef PLAYER_MODULE
#define PLAYER_MODULE

#include <ncurses.h>

class player
{
    int x, y, MaxX, MaxY, PlayerHigh, PlayerWidth;
public:
    player(int PlayerH, int PlayerW);
    void showPlayer();
    void hidePlayer();
    void setPosition(int NewY, int NewX);
};

player::player(int PlayerH, int PlayerW)
{
    PlayerHigh = PlayerH;
    PlayerWidth = PlayerW;
    getmaxyx(stdscr, MaxY, MaxX);
    x = MaxX/2;
    y = MaxY - PlayerHigh;
}

void player::showPlayer()
{
    for(int i = y; i < y + PlayerHigh; i++)
    {
        for (int n = x; n < x + PlayerWidth; n++)
        {
            move(i, n);
            addch('*');
        }
    }
}

void player::hidePlayer()
{
    for (int i = y-1; i < y + PlayerHigh+1; i++)
    {
        for (int n = x-1; n < x + PlayerWidth+1; n++)
        {
            move(i, n);
            addch(' ');
        }
    }
}

void player::setPosition(int NewY, int NewX)
{
    y = NewY;
    x = NewX;
}

#endif