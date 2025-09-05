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
        int *tmp = new int; 
        *tmp = 0;        
        value = tmp;
        type = INT;
        delete tmp;
    }   
    
    typeless(void *val, Type typ)
    {
        value = new void*;
        value = val;
        type = typ;
    }

    typeless(const typeless& copy)
    {
        printf("sbjkfas\n");
        Vector VectorValue;
        int intValue;
        value = new void*;
        switch (copy.type)
        {
        case INT:
            intValue = *static_cast<int*>(value);
            printf("value: %d\n", intValue);
            value = &intValue; 
            break;
        case VECTOR:
            VectorValue = *static_cast<Vector*>(value);
            printf("value:%d\n", VectorValue.x);
            value = &VectorValue; 
            break;    
        default:
            break;
        }
        type = copy.type;
    }   

    void setValue(void *val, Type typ)      
    {
        value = val;
        type = typ;
    }

    ~typeless()
    {
        delete value;
    }

}typeless;

#endif