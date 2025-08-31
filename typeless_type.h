#ifndef TYPELESS_MODULE
#define TYPELESS_MODULE

#include "vector.h"

enum Type
{
    INT,
    VECTOR
};

typedef struct typeless
{
    Type type;
    void *value;

    typeless(void *val, Type typ)
    {
        value = val;
        type = typ;
    }

    typeless()
    {
        value = NULL;
    }

    void setValue(void *val, Type typ)
    {
        value = val;
        type = typ;
    }
}typeless;

#endif