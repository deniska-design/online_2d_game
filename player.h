#ifndef PLAYER_MODULE
#define PLAYER_MODULE

#include "object.h"


enum 
{
	alive	=	active,      
    dead	=	disactiv,      

    DefaultPositionX= 0,
    DefaultPositionY= 0,
    DefaultHigh     = 5,
    DefaultWidth    = 2,
    DefaultHP       = 3,  
};

class player : public object 
{
    int HP;
public:
    player();
    player(int objectH, int objectW, Vector NewPosition, int NewHP);
};

player::player()
{
    position = Vector{DefaultPositionY, DefaultPositionY};
    High = DefaultHigh;
    Width = DefaultWidth;
    HP = DefaultHP;
    statue = disactiv;
}

player::player(int PlayerH, int PlayerW, Vector NewPosition, int NewHP)
{
    position = NewPosition;
    High = PlayerH;
    Width = PlayerW;
    statue = disactiv;
    HP = NewHP;
}

#endif