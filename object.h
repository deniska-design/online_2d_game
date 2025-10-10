#ifndef OBJECT_MODULE
#define OBJECT_MODULE

#include <ncurses.h>

enum 
{
	active	    =	true,     
    disactiv	=	false,  
    
    DefaultPositionX= 25,
    DefaultPositionY= 25,
    
    DefaultHigh     = 5,
    DefaultWidth    = 2,
};

typedef enum 
{
	PlayerType,
    BombType
}objectType;

class object
{
protected:
    Vector position;
    int High, Width;
    bool statue;
    objectType ObjectType;

public:
    object();
    object(int objectH, int objectW, Vector NewPosition);
    void Show() const;
    void Hide() const;
    int &GetY();
    int &GetX();
    int &GetHigh();
    int &GetWidth();
    void setPosition(int NewY, int NewX);
    void setStatue(bool newStatue);
    bool getStatue() const;
    void setType(objectType NewObjectType);
    objectType getType() const;
};

object::object()
{
    position = Vector{DefaultPositionX, DefaultPositionY};
    High = DefaultHigh;
    Width = DefaultWidth;
    statue = disactiv;
}

object::object(int PlayerH, int PlayerW, Vector NewPosition)
{
    position = NewPosition;
    High = PlayerH;
    Width = PlayerW;
    statue = disactiv;
}

void object::Show() const
{
    for(int i = position.y; i < position.y + High; i++)
    {
        for (int n = position.x; n < position.x + Width; n++)
        {
            move(i, n);
            addch('*');
        }
    }
}

void object::Hide() const
{
    for (int i = position.y-3; i < position.y + High+3; i++)
    {
        for (int n = position.x-3; n < position.x + Width+3; n++)
        {
            move(i, n);
            addch(' ');
        }
    }
}

void object::setPosition(int NewY, int NewX)
{
    position.y = NewY;
    position.x = NewX;
}

int &object::GetY()
{
    return const_cast<int &>(position.y);
}

int &object::GetX()
{
    return const_cast<int &>(position.x);
}

void object::setStatue(bool newStatue)
{
    statue = newStatue;
}

bool object::getStatue() const
{
    return statue;
}

void object::setType(objectType NewObjectType)
{
    ObjectType = NewObjectType;
}

objectType object::getType() const
{
    return ObjectType;
}

int &object::GetHigh()
{
    return const_cast<int &>(High);
}

int &object::GetWidth()
{
    return const_cast<int &>(Width);
}

#endif