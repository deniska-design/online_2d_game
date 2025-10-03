#ifndef PLAYER_MODULE
#define PLAYER_MODULE

#include "object.h"


enum 
{
	alive	    =	active,      
    dead	    =	disactiv,      

    DefaultHP   = 3,  
};

class player : public object 
{
    int HP;
public:
    player();
    player(int objectH, int objectW, Vector NewPosition, int NewHP);
    int &GetHP();
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


int &player::GetHP()
{   
    return (int &)HP;
}


#endif