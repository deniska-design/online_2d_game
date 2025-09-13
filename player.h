#ifndef PLAYER_MODULE
#define PLAYER_MODULE

#include <ncurses.h>

enum 
{
	alive	=	true,      // Key: Cursor right
    dead	=	false,      // Key: Cursor left
};

class player
{
    Vector position;
    int PlayerHigh, PlayerWidth;
    bool AliveOrNot;
public:
    player();
    player(int PlayerH, int PlayerW);
    void showPlayer() const;
    void hidePlayer() const;
    int &GetY();
    int &GetX();
    Vector &GetPosition();
    void setPosition(int NewY, int NewX);
    void setStatue(bool newStatue);
    bool getStatue();
};

player::player()
{
    PlayerHigh = 5;
    PlayerWidth = 2;
    AliveOrNot = alive;
}

player::player(int PlayerH, int PlayerW)
{
    PlayerHigh = PlayerH;
    PlayerWidth = PlayerW;
    AliveOrNot = alive;
}

void player::showPlayer() const
{
    if (AliveOrNot == alive)
    {
        for(int i = position.y; i < position.y + PlayerHigh; i++)
        {
            for (int n = position.x; n < position.x + PlayerWidth; n++)
            {
                move(i, n);
                addch('*');
            }
        }
    }
}

void player::hidePlayer() const
{
    for (int i = position.y-1; i < position.y + PlayerHigh+1; i++)
    {
        for (int n = position.x-1; n < position.x + PlayerWidth+1; n++)
        {
            move(i, n);
            addch(' ');
        }
    }
}

void player::setPosition(int NewY, int NewX)
{
    position.y = NewY;
    position.x = NewX;
}

int &player::GetY()
{
    return const_cast<int &>(position.y);
}

int &player::GetX()
{
    return const_cast<int &>(position.x);
}

Vector &player::GetPosition() 
{
    return const_cast<Vector &>(position);
}

void player::setStatue(bool newStatue)
{
    AliveOrNot = newStatue;
}

bool player::getStatue()
{
    return AliveOrNot;
}

#endif