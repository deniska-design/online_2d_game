#ifndef PLAYER_MODULE
#define PLAYER_MODULE

#include "object.h"


enum 
{
	alive	=	active,      
    dead	=	disactiv,      
};

class player : public object 
{
public:
    player();
    player(int objectH, int objectW, Vector NewPosition);
};

player::player()
{
    position = Vector{0, 0};
    High = 5;
    Width = 2;
    statue = disactiv;
}

player::player(int PlayerH, int PlayerW, Vector NewPosition)
{
    position = NewPosition;
    High = PlayerH;
    Width = PlayerW;
    statue = disactiv;
}

#endif