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

    typeless()  
    {   
        value = new void*;
        value = (int *)0;        
        type = INT;
    }   
    
    typeless(void *val, Type typ)
    {
        value = new void*;
        value = val;
        type = typ;
    }

    void setValue(void *val, Type typ)      
    {
        value = val;
        type = typ;
    }
}typeless;

#endif