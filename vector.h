#ifndef VECTOR_MODULO
#define VECTOR_MODULO

typedef struct Vector 
{
   int x;                
   int y;     

   bool operator==(Vector arg)
   {
      if((this->x == arg.x) && (this->y == arg.y))
      {
         return true;
      }
      return false;
   }  
   
   Vector operator+ (Vector addition)
   {
      Vector tmp;
      tmp.x = this->x + addition.x;
      tmp.y = this->y + addition.y;
      return tmp;
   }

   Vector operator- (Vector Subtrahend)
   {
      Vector tmp;
      tmp.x = this->x - Subtrahend.x;
      tmp.y = this->y - Subtrahend.y;
      return tmp;
   }
   Vector operator/ (int divider)
   {
      Vector tmp;
      tmp.x = this->x / divider;
      tmp.y = this->y / divider;
      return tmp;
   }
   bool operator> ( Vector arg)
   {
    if((this->x > arg.x) && (this->y > arg.y))
      {
         return true;
      }
      return false;
   }
} Vector;
#endif