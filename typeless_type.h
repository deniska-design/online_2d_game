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
    int jopa = 0;

    typeless()  
    {   
        value = new void*;
        tmp = new int; 
        *tmp = jopa;        
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