#ifndef LIST_MODULO
#define LIST_MODULO

//definition:
#include "stdio.h"

template<class T> 
class OnceLinkedList
{
    public:
    class Item
    {
        public:
        T node;
        Item *next;
    };

    Item *item;

	OnceLinkedList();

    void CreateNextElem();

    void DeleteNextElem();

    void Assign(T RightOperand);

    void GoToNextElem();
};



//implement:
template<class T> 
class DoubleLinkedList
{
    public:
    class Item
    {
        public:
        T node;
        Item *next;
        Item *prev;
    };

    Item *item;

	DoubleLinkedList();

    void CreateNextElem();

    void CreatePrevElem();

    void DeleteElem();

    void Assign(T RightOperand);

    void GoToNextElem();

    void GoToPrevElem();

    void GoToFirstElem();
};

//OnceLinkedList:
template<class T> 
    OnceLinkedList<T>::OnceLinkedList()
    {
        OnceLinkedList<T>::item = new OnceLinkedList<T>::Item;
    }
template<class T> 
    void OnceLinkedList<T>::CreateNextElem()
    {
        OnceLinkedList<T>::item->next = new OnceLinkedList<T>::Item;   
    }
template<class T> 
    void OnceLinkedList<T>::DeleteNextElem()
    {
        OnceLinkedList<T>::item->next = OnceLinkedList<T>::item->next->next;   
    }
template<class T> 
    void OnceLinkedList<T>::Assign(T RightOperand)
    {
        OnceLinkedList<T>::item->node = RightOperand; 
    }
template<class T> 
    void OnceLinkedList<T>::GoToNextElem()
    {
        OnceLinkedList<T>::item = OnceLinkedList<T>::item->next;
    }


//DoubleLinkedList:
template<class T> 
    DoubleLinkedList<T>::DoubleLinkedList()
    {
        DoubleLinkedList<T>::item = new DoubleLinkedList<T>::Item;
        DoubleLinkedList<T>::item->prev = NULL;
        DoubleLinkedList<T>::item->next = NULL;
    }
template<class T> 
    void DoubleLinkedList<T>::CreateNextElem()
    {
        DoubleLinkedList<T>::item->next = new DoubleLinkedList<T>::Item;   
        DoubleLinkedList<T>::item->next->prev = DoubleLinkedList<T>::item;
    }
template<class T> 
    void DoubleLinkedList<T>::CreatePrevElem()
    {
        DoubleLinkedList<T>::item->prev = new DoubleLinkedList<T>::Item;   
        DoubleLinkedList<T>::item->prev->next = DoubleLinkedList<T>::item; 
    }
template<class T> 
    void DoubleLinkedList<T>::DeleteElem()
    {
        DoubleLinkedList<T>::item = DoubleLinkedList<T>::item->next;  
        DoubleLinkedList<T>::item->prev = DoubleLinkedList<T>::item->prev->prev;
    }
template<class T> 
    void DoubleLinkedList<T>::Assign(T RightOperand)
    {
        DoubleLinkedList<T>::item->node = DoubleLinkedList<T>::RightOperand; 
    }
template<class T> 
    void DoubleLinkedList<T>::GoToNextElem()
    {
        DoubleLinkedList<T>::item = DoubleLinkedList<T>::item->next;
    }
template<class T> 
    void DoubleLinkedList<T>::GoToPrevElem()
    {
        DoubleLinkedList<T>::item = DoubleLinkedList<T>::item->prev;
    }
template<class T> 
    void DoubleLinkedList<T>::GoToFirstElem()
    {
        while(DoubleLinkedList<T>::item->prev)
        {
            DoubleLinkedList<T>::GoToPrevElem();
        }
    }

#endif