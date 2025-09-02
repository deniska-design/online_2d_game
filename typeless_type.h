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
    int *tmp;

    typeless()  
    {   
        value = new void*;
        tmp = new int; 
        *tmp = 0;        
        value = tmp;
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