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
        value = (int*)1;       //что это нахуй такое 
        type = INT;
    }   

    void setValue(void *val, Type typ)      //твой консруктор залупа
    {
        value = val;
        type = typ;
    }
}typeless;

#endif