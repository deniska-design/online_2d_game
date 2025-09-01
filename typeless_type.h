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
        int *tmp = new(int);
        *tmp = 0;
        value = tmp;        
        type = INT;
    }   

    void setValue(void *val, Type typ)      
    {
        value = val;
        type = typ;
    }
}typeless;

#endif