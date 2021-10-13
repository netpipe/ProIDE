/*
 * Memory_Pool.cpp: Routines for organising allocations of memory
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/
                    
using namespace std;                         
                         
#include "memory_pool.hh"
#include <cstring>

mem::Memory_Pool::Memory_Pool()
{
    cout << "Constructor Memory_Pool()" << "\n";
    
    available = 0;
    current = 0;
    
    search_previous_offset = search_previous_start = 0;  
    search_previous_direction = false;  

    pool_size = DEFAULT_POOL_SIZE;
}

mem::Memory_Pool::Memory_Pool(long Pool_Size)
{
    available = 0;
    current = 0;

    search_previous_offset = 0;  
    search_previous_direction = false;  

    if (Pool_Size == 0)
    {
        pool_size = DEFAULT_POOL_SIZE;
    }
    else
    {
        pool_size = Pool_Size;
    }                                           
}


mem::Memory_Pool::~Memory_Pool()
{
    uint i;

    /////////////////////////////////////////////////

    cout << "Destructor Memory_Pool()" << "\n";

    for (i = 0; i < pool_list.size(); i++)
    { free(pool_list[i].memory_offset); }
}

void *
mem::Memory_Pool::Use_Pool(long size)
{
    char *      temp;
    Memory_Node mn;

    ////////////////////////////////////////////////////////////////////////////

    if (available >= size)                  // enough memory available in pool
    {                                        
      available -= size; 
      temp = current;
      current += size;

      return temp;
    }
                                           // not enough memory available in pool
    temp = (char *) malloc (pool_size);
    
    if (temp)
    { 
      current = temp + size;

      mn.memory_offset = temp;
      mn.associated_value = associated_value;
      mn.size = pool_size;

      pool_list.push_back(mn);

      available = pool_size - size; 

      return temp;
    }

    available = 0;
     
    return 0;  
}

void 
mem::Memory_Pool::Set_Pool_Size(int size)
{
    pool_size = size;
}


void  
mem::Memory_Pool::Reset_Available()
{
    available = 0;
}

void 
mem::Memory_Pool:: Set_Associated_Value(int associated)
{
    associated_value = associated;
}
    
bool 
mem::Memory_Pool::Search_Memory_Pool(bool direction, bool wrap, long start_value,
                                     std::vector<char> *search_byte, long * offset_found)
{
   bool     found,           // was search_string found?
            wrapped,         // did search wrap?
            previous,        // take previous search into account?
            stop;       

   uint     i;               // index

   long     j, l, n, m, w;   // temp value, length, number of bytes still to search, max bytes in list, wrap

   ///////////////////////////////////////////////////////////////////

   wrapped = found = stop = false;

   * offset_found = RET_ERR_GENERAL;

   i = 0;
   while (pool_list[i].associated_value + pool_list[i].size < start_value)
   { i++; }

   j = start_value - pool_list[i].associated_value;

   m = n = pool_list[i].size; 

   if (j >= n)
   { return  false; }

   previous = true;
   if (direction != search_previous_direction)
   { previous = false;} 
   else 
   {
    if (direction)
    {
     if (start_value < search_previous_start)
     { previous = false; }
    }
    else if (start_value > search_previous_start)
     { previous = false; }
   } 
   
   l = search_byte->size();             
   
   if (l == 0)
   { return  false; }

   if (direction)
   { n -= j; }
   else
   { n  = j; }                                 // n = available chars in memory pool

   current = pool_list[i].memory_offset;
   current += j;

   w = start_value;
   
   while (!stop)
   {
    j = 0;         
    found = true;  
    

    if (direction)                             // search forward
    {
      while ((j < l) && (found))
      {                                        // compare pool with target values, l = length of search
       if (* ((char *) current + j) == (*search_byte)[j])
       { j++; }
       else
       { found = false; }
      }   

      n--;    
      if (n < l)
      { found = false; }

      if (found)
      { 
       *offset_found = pool_list[i].associated_value + pool_list[i].size - n - 1;
       if ((*offset_found <= search_previous_offset) && (!wrapped) && (previous))
       {
        found = false;                         // if we don't do this two consecutive searches will give same result
       }              
      }               

      if (!found)
      {
       (current)++;
       if ((wrapped) && (start_value <= w))
       { stop = true; }

       if (n <= 0)
       {
        i++;    
        if (i >= pool_list.size())
        {
         if (wrap)                  
         { 
          i = 0;
          wrapped = true;
         }
         else
         {stop = true;}
        }
        m = n = pool_list[i].size;
        w = pool_list[i].associated_value;
        current = pool_list[i].memory_offset;
       }
      }

      else                                  // if found
      { stop = true; }
    }
    
     ///////////////////////////////////////////////////////////////////////////////
    
    else                                // if (!direction): search backward
    { 
      while ((j < l) && (found))
      {
       if (* ((char *) current + j) == (*search_byte)[j])
       { j++; }
       else
       { found = false; }
      }   

      n--;    
      if (n > m - l)
      { found = false; }

      if (found)
      { 
       *offset_found = pool_list[i].associated_value + n + 1;
       if ((*offset_found >= search_previous_offset) && (!wrapped) && (previous))
       {
        found = false;  
       }              
      }                    

      if (!found)
      {
       (current)--;
       if ((wrapped) && (start_value >= w))
       { stop = true; }

       if (n <= 0)
       {
        if (i == 0)
        {
         if (wrap) 
         { 
          i = pool_list.size() - 1 ;
          wrapped = true;
         }
         else
         {stop = true;}
        }
        else
        {
         i--;
        }
        m = n = pool_list[i].size;
        current = ((char *) pool_list[i].memory_offset) + pool_list[i].size - 1;
        w = pool_list[i].associated_value + pool_list[i].size;
       }
      }

      else                                  // if found
      {
       stop = true;
      }
    }  
   }

   //debug
   //if (found) {cout  << "search string " << *search_byte << " found at " << (* offset_found) << "\n";}

   search_previous_offset = * offset_found;
   search_previous_direction = direction;
   search_previous_start = start_value;

   return found;
}

void 
mem::Memory_Pool:: Clear()
{
   uint i;

   /////////////////////////////////////////////////

   for (i = 0; i < pool_list.size(); i++)
   { free(pool_list[i].memory_offset); }
}

void *
mem::Memory_Pool::Ensure_Minimum_Allocation(char *allocated, uint minimum)
{
    if (   (allocated == 0)                            
        || (strlen(allocated) <= minimum))                        // velsd: tough luck for the memory wasted 
     { 
      return Use_Pool(minimum + 1);
     }       

    return allocated;
}



