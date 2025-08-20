#ifndef PLAYER_MODULE
#define PLAYER_MODULE

#include <ncurses.h>

class player
{
private:
    int x, y, MaxX, MaxY, PlayerHigh, PlayerWidth;
public:
    player(int PlayerH, int PlayerW);
    void showPlayer();
    void hidePlayer();
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
    for (int i = y; i < y + PlayerHigh; i++)
    {
        for (int n = x; n < x + PlayerWidth; n++)
        {
            move(i, n);
            addch(' ');
        }
    }
}

#endif