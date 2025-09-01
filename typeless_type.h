#ifndef TYPELESS_MODULE
#define TYPELESS_MODULE

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
        value = new void*;
        value = (int *)0;        
        type = INT;
    }   

    void setValue(void *val, Type typ)      
    {
        value = val;
        type = typ;
    }
}typeless;

#endif