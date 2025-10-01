#include "list.h"

int main() 
{
    DoubleLinkedList<int> *list = new DoubleLinkedList<int>;
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for(int i = 0; i < 10; i++)
      {
        list->item->node = array[i];
        list->CreateNextElem();
        list->GoToNextElem();
      }

      for(int i = 0; i < 10; i++)
      {
        list->GoToPrevElem();
        printf("%d\n", list->item->node);
      }
    return 0;
}