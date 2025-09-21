#ifndef OBJECT_MODULE
#define OBJECT_MODULE

#include <ncurses.h>

enum 
{
	active	    =	true,     
    disactiv	=	false,      
};

class object
{
protected:
    Vector position;
    int High, Width;
    bool ActiveOrNot;
public:
    object();
    object(int objectH, int objectW, Vector NewPosition);
    void Show() const;
    void Hide() const;
    int &GetY();
    int &GetX();
    Vector &GetPosition();
    void setPosition(int NewY, int NewX);
    void setStatue(bool newStatue);
    bool getStatue() const;
    int getSize();
};

object::object()
{
    position = Vector{0, 0};
    High = 5;
    Width = 2;
    ActiveOrNot = disactiv;
}

object::object(int PlayerH, int PlayerW, Vector NewPosition)
{
    position = NewPosition;
    High = PlayerH;
    Width = PlayerW;
    ActiveOrNot = disactiv;
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
    for (int i = position.y-1; i < position.y + High+1; i++)
    {
        for (int n = position.x-1; n < position.x + Width+1; n++)
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

Vector &object::GetPosition() 
{
    return const_cast<Vector &>(position);
}

void object::setStatue(bool newStatue)
{
    ActiveOrNot = newStatue;
}

bool object::getStatue() const
{
    return ActiveOrNot;
}

int object::getSize()
{
    return High;
}

#endif