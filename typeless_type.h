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
        value = new void*;
        switch (copy.type)
        {
        case INT:
            value = static_cast<int*>(value);
            break;
        case VECTOR:
            value = static_cast<Vector*>(value);
            break;    
        default:
            break;
        }
        value = copy.value;     //я присваиваю указателю указатель. В этом конструкторе нет смысла
        type = copy.type;
    }   

    void setValue(void *val, Type typ)      
    {
        value = val;
        type = typ;
    }

    ~typeless()
    {
        switch (type)
        {
        case INT:
            value = static_cast<int*>(value);
            break;
        case VECTOR:
            value = static_cast<Vector*>(value);
            break;    
        default:
            break;
        }
        delete value;
    }

}typeless;

#endif